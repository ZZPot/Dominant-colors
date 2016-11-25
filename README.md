# Dominant-colors
Extract dominant/palette colors from image (OpenCV).

##Info
Histogram can be RGB or HSV. Near colors calculating via cube-space or CIE76 color difference.

RGB histogram good for vivid/limited colors, like animation or retro video-games. Example provided.

HSV histogram good for natural colors, like landscapes. Example provided.

NOTE: function GetCenter() has "val = 0;" code. Commenting it leads to changing results in both directions. Didn't decide what to do with that.

CIE distance, cube size and histogram bins are set experimentally. Algorithm is still plastic lame and weak.