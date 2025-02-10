//@version=5
strategy("PriceAction_ver2 by Zhangjingyi", overlay=true, default_qty_type=strategy.percent_of_equity, default_qty_value=100)

// 🟢 Input 
smaLength = input.int(200, title="SMA Length", minval=1)
emaLength = input.int(21, title="EMA Length", minval=1)
riskPercentage = input.float(0.5, title="Risk Percentage per Trade", minval=0.1)  // 0.5% risk per trade

// 🔵 SMA and EMA
sma200 = ta.sma(close, smaLength)
ema21 = ta.ema(close, emaLength)

// 🟠 Define Engulfing Patterns
bullishEngulfingFirst = (close[1] < open[1] and close[1] > open[2]) and (close > open and close > open[1])
bullishEngulfingSecond = close[3] < open[3] and (close[2] > open[2] and close[2] > open[3])
bearishEngulfingFirst = (close[1] > open[1] and close[1] < open[2]) and (close < open and close < open[1])
bearishEngulfingSecond = close[3] > open[3] and close[2] < open[2] and close[2] < open[3]

bullishConditionFirst = (open[1] - close[1]) > (close - open) * 0.3
bullishConditionSecond = (open[3] - close[3]) > (close[2] - open[2]) * 0.3
bearishConditionFirst = (close[1] - open[1]) > (open - close) * 0.3
bearishConditionSecond = (close[3] - open[3]) > (open[2] - close[2]) * 0.3

// 🟢 Buy/Sell conditions
buySignalContinueEngulfing = bullishEngulfingFirst and bullishEngulfingSecond and (bullishConditionFirst or bullishConditionSecond) and close > ema21
sellSignalContinueEngulfing = bearishEngulfingFirst and bearishEngulfingSecond and (bearishConditionFirst or bearishConditionSecond) and close < ema21

// 🟠 Define 3 Pairs Engulfing Patterns
bullishPairFirst = close[5] < open[5] and close[4] > open[4] and close[4] > open[5]
bearishPairSecond = close[3] > open[3] and close[2] < open[2] and close[2] < open[3]
bullishPairThird = close[1] < open[1] and close > open and close > open[2] 

bearishPairFirst = close[5] > open[5] and close[4] < open[4]  and close[4] < open[5]
bullishPairSecond = close[3] < open[3] and close[2] > open[2] and close[2] > open[3]
bearishPairThird = close[1] > open[1] and close < open and close < open[2]

// 🟢 Buy/Sell conditions for 3 Pairs
buySignal3PairsEngulfing = bullishPairFirst and bearishPairSecond and bullishPairThird 
sellSignal3PairsEngulfing = bearishPairFirst and bullishPairSecond and bearishPairThird

// 🟡 Calculate Stop Loss and Take Profit
stopLossBuy = ta.lowest(low, 4)  // SL for buySignalContinueEngulfing
stopLossSell = ta.highest(high, 4)  // SL for sellSignalContinueEngulfing

stopLossBuy3Pairs = ta.lowest(low, 2)  // SL for buySignal3PairsEngulfing
stopLossSell3Pairs = ta.highest(high, 2)  // SL for sellSignal3PairsEngulfing

// Take Profit (TP = 2x SL)
takeProfitBuy = close + (close - stopLossBuy) * 2
takeProfitSell = close - (stopLossSell - close) * 2
takeProfitBuy3Pairs = close + (close - stopLossBuy3Pairs) * 2
takeProfitSell3Pairs = close - (stopLossSell3Pairs - close) * 2

// 🟣 Calculate Position Size
positionSizeBuy = (riskPercentage / 100) * strategy.equity / (close - stopLossBuy)
positionSizeSell = (riskPercentage / 100) * strategy.equity / (stopLossSell - close)
positionSizeBuy3Pairs = (riskPercentage / 100) * strategy.equity / (close - stopLossBuy3Pairs)
positionSizeSell3Pairs = (riskPercentage / 100) * strategy.equity / (stopLossSell3Pairs - close)

// 🟠 Avoid Opening New Orders If There Are Active Trades
isPositionOpen = strategy.position_size != 0

// ✅ Execute Trades
if buySignalContinueEngulfing and not isPositionOpen
    strategy.entry("Buy", strategy.long, qty=positionSizeBuy)
    strategy.exit("Take Profit/Stop Loss", "Buy", stop=stopLossBuy, limit=takeProfitBuy)

if sellSignalContinueEngulfing and not isPositionOpen
    strategy.entry("Sell", strategy.short, qty=positionSizeSell)
    strategy.exit("Take Profit/Stop Loss", "Sell", stop=stopLossSell, limit=takeProfitSell)

if buySignal3PairsEngulfing and not isPositionOpen  
    strategy.entry("Buy_3Pairs", strategy.long, qty=positionSizeBuy3Pairs)
    strategy.exit("Take Profit/Stop Loss", "Buy_3Pairs", stop=stopLossBuy3Pairs, limit=takeProfitBuy3Pairs)

if sellSignal3PairsEngulfing and not isPositionOpen
    strategy.entry("Sell_3Pairs", strategy.short, qty=positionSizeSell3Pairs)
    strategy.exit("Take Profit/Stop Loss", "Sell_3Pairs", stop=stopLossSell3Pairs, limit=takeProfitSell3Pairs)

// 🔔 Alerts
alertcondition(buySignalContinueEngulfing or buySignal3PairsEngulfing, title="BUY Alert", message="BUY Signal detected")
alertcondition(sellSignalContinueEngulfing or sellSignal3PairsEngulfing, title="SELL Alert", message="SELL Signal detected")

// 📍 Show BUY/SELL on chart
if (buySignalContinueEngulfing or buySignal3PairsEngulfing) and not isPositionOpen
    label.new(bar_index, high, text="BUY", color=color.green, style=label.style_label_up, textcolor=color.white)

if (sellSignalContinueEngulfing or sellSignal3PairsEngulfing) and not isPositionOpen
    label.new(bar_index, low, text="SELL", color=color.red, style=label.style_label_down, textcolor=color.white)

// 📊 Draw SMA and EMA lines
plot(sma200, color=color.yellow, title="SMA 200")
plot(ema21, color=color.blue, title="EMA 21")
