//@version=5
// 📌 This script is an improved version of the PriceAction_ver1 script
// 📌 It uses the same logic as PriceAction_ver1 but with additional features
// 📌 It includes alerts for Buy/Sell signals and position size calculation
// 📌 It also includes the ability to draw Buy/Sell signals on the chart
// 📌 The script is designed to work on the 1H timeframe for Forex pairs and 4H timeframe for Crypto pairs

strategy("PriceAction_ver1 by Zhangjingyi", overlay=true, default_qty_type=strategy.percent_of_equity, default_qty_value=100)

// 🟢 Input 
smaLength = input.int(200, title="SMA Length", minval=1)
emaLength = input.int(21, title="EMA Length", minval=1)
riskPercentage = input.float(0.5, title="Risk Percentage per Trade", minval=0.1)  // 0.5% risk per trade

// 🔵 SMA and EMA
sma200 = ta.sma(close, smaLength)
ema21 = ta.ema(close, emaLength)

// 🔶 Define Engulfing Patterns
bullishEngulfingFirst = (close[1] < open[1] and close[1] > open[2]) and (close > open and high > high[1] and close > open[1])
bullishEngulfingSecond = close[3] < open[3] and (close[2] > open[2] and high[2] > high[3] and close[2] > open[3])
bullishConditionFirst = (open[1] - close[1]) > (close - open) * 0.3
bullishConditionSecond = (open[3] - close[3]) > (close[2] - open[2]) * 0.3

bearishEngulfingFirst = (close[1] > open[1] and close[1] < open[2]) and (close < open and close < open[1] and open > close[1])
bearishEngulfingSecond = close[3] > open[3] and close[2] < open[2] and close[2] < open[3] and open[2] > close[3]
bearishConditionFirst = (close[1] - open[1]) > (open - close) * 0.3
bearishConditionSecond = (close[3] - open[3]) > (open[2] - close[2]) * 0.3

// 🟢 Buy condition
buySignal = bullishEngulfingFirst and bullishEngulfingSecond and close > ema21 and bullishConditionFirst and bullishConditionSecond

// 🔴 Sell condition
sellSignal = bearishEngulfingFirst and bearishEngulfingSecond and close < ema21

// 🟡 Calculate Stop Loss and Take Profit
stopLossBuy = ta.lowest(low, 4)  // Lowest Stop Loss in the last 4 candles (Buy)
stopLossSell = ta.highest(high, 4)  // Highest Stop Loss in the last 4 candles (Sell)

takeProfitBuy = close + (close - stopLossBuy) * 2  // R:R = 2:1 for Buy
takeProfitSell = close - (stopLossSell - close) * 2  // R:R = 2:1 for Sell

// 🟣 Calculate Position Size
positionSizeBuy = (riskPercentage / 100) * strategy.equity / (close - stopLossBuy)
positionSizeSell = (riskPercentage / 100) * strategy.equity / (stopLossSell - close)

// ✅ Execute Trades
if buySignal
    strategy.entry("Buy", strategy.long, qty=positionSizeBuy)
    strategy.exit("Take Profit/Stop Loss", "Buy", stop=stopLossBuy, limit=takeProfitBuy)

if sellSignal
    strategy.entry("Sell", strategy.short, qty=positionSizeSell)
    strategy.exit("Take Profit/Stop Loss", "Sell", stop=stopLossSell, limit=takeProfitSell)

// 🔎 Determine the time frame and asset type (currency pair)
isH1 = timeframe.period == "60"  // 1H
isH4 = timeframe.period == "240"  // 4H

isForex = syminfo.ticker == "GBPUSD" or syminfo.ticker == "EURUSD"
isCrypto = syminfo.ticker == "ETHUSD" or syminfo.ticker == "BTCUSD" 
isGold = syminfo.ticker == "XAUUSD"
// 🔔 Enable alerts only for correct currency pair and timeframe
enableAlert = (isForex and isH1) or (isCrypto and isH4) or (isGold and isH4)

// 🔔 Create alerts when there is a Buy/Sell signal
alertcondition(buySignal and enableAlert, title="BUY Alert", message="BUY Signal on {syminfo.ticker} - {timeframe.period}")
alertcondition(sellSignal and enableAlert, title="SELL Alert", message="SELL Signal on {syminfo.ticker} - {timeframe.period}")

// 📍 Show BUY/SELL on chart
if buySignal and enableAlert
    label.new(bar_index, high, text="BUY", color=color.green, style=label.style_label_up, textcolor=color.white)

if sellSignal and enableAlert
    label.new(bar_index, low, text="SELL", color=color.red, style=label.style_label_down, textcolor=color.white)

// 📊 Draw SMA and EMA lines
plot(sma200, color=color.yellow, title="SMA 200")
plot(ema21, color=color.blue, title="EMA 21")
