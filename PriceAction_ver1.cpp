//@version=5
strategy("Enhanced Engulfing Strategy with SMA200 & EMA21", overlay=true, default_qty_type=strategy.percent_of_equity, default_qty_value=100)

// Inputs
smaLength = input.int(200, title="SMA Length", minval=1)
emaLength = input.int(21, title="EMA Length", minval=1)
riskPercentage = input.float(0.5, title="Risk Percentage per Trade", minval=0.1)  // 0.5% risk per trade

// Calculate SMA and EMA
sma200 = ta.sma(close, smaLength)
ema21 = ta.ema(close, emaLength)

// Define Engulfing Patterns
bullishEngulfingFirst = (close[1] < open[1] and close[1] > open[2]) and (close > open and high > high[1] and close > open[1])
bullishEngulfingSecond = close[3] < open[3] and (close[2] > open[2] and high[2] > high[3] and close[2] > open[3])
bearishEngulfingFirst = (close[1] > open[1] and close[1] < open[2]) and (close < open and close < open[1] and open > close[1])
bearishEngulfingSecond = close[3] > open[3] and close[2] < open[2] and close[2] < open[3] and open[2] > close[3]

// Buy condition: Bullish Engulfing + price above EMA21
buySignal = bullishEngulfingFirst and bullishEngulfingSecond and close > ema21

// Sell condition: Bearish Engulfing + price below EMA21
sellSignal = bearishEngulfingFirst and bearishEngulfingSecond and close < ema21

// Calculate Stop Loss and Take Profit
stopLossBuy = ta.lowest(low, 4)  // Lowest low of the last 4 candles for Buy
stopLossSell = ta.highest(high, 4)  // Highest high of the last 4 candles for Sell

takeProfitBuy = close + (close - stopLossBuy) * 2  // R:R = 2:1 for Buy
takeProfitSell = close - (stopLossSell - close) * 2  // R:R = 2:1 for Sell

// Calculate Position Size
positionSizeBuy = (riskPercentage / 100) * strategy.equity / (close - stopLossBuy)
positionSizeSell = (riskPercentage / 100) * strategy.equity / (stopLossSell - close)

// Execute Trades
if buySignal
    strategy.entry("Buy", strategy.long, qty=positionSizeBuy)
    strategy.exit("Take Profit/Stop Loss", "Buy", stop=stopLossBuy, limit=takeProfitBuy)

if sellSignal
    strategy.entry("Sell", strategy.short, qty=positionSizeSell)
    strategy.exit("Take Profit/Stop Loss", "Sell", stop=stopLossSell, limit=takeProfitSell)

// Plot SMA and EMA
plot(sma200, color=color.yellow, title="SMA 200")
plot(ema21, color=color.blue, title="EMA 21")

// Debugging Info: Add Labels for Buy/Sell Signals
if buySignal
    label.new(bar_index, high, text="Buy", color=color.green, style=label.style_label_up, textcolor=color.white)
if sellSignal
    label.new(bar_index, low, text="Sell", color=color.red, style=label.style_label_down, textcolor=color.white)