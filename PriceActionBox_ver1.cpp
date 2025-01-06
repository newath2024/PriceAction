//@version=5
indicator("Engulfing Boxes", overlay=true)

// Define Engulfing Patterns
bullishEngulfing = close[1] < open[1] and close > open and close > open[1] and open < close[1]  // Bullish Engulfing
bearishEngulfing = close[1] > open[1] and close < open and close < open[1] and open > close[1]  // Bearish Engulfing

// Draw Bullish Engulfing Box
if bullishEngulfing
    box.new(left=bar_index - 1, top=open[1], right=bar_index, bottom=math.min(low[1], low), border_color=color.green, bgcolor=color.new(color.green, 85))

// Draw Bearish Engulfing Box
if bearishEngulfing
    box.new(left=bar_index - 1, top=math.max(high[1], high), right=bar_index, bottom=open[1], border_color=color.red, bgcolor=color.new(color.red, 85))