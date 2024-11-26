const static char html_main_page[] PROGMEM = R"=====(
 <!DOCTYPE html>
<html>
<head>
	<title></title>
	<meta charset='utf-8'>
</head>
<body>
<center><h1>Комфортный дом </h1></center>
<br>
<br><br><br>
<input type='button' value='Лампы включить' style='width:300Px;height:200Px' onclick="location.href='leds?lamp=3';">
<input type='button' value='Лампы потушить' style='width:300Px;height:200Px' onclick="location.href='leds?lamp=0';">

<br><br><br>
<input type='button' value='1 Лампа' style='width:300Px;height:100Px' onclick="location.href='leds?lamp=1';">
<input type='button' value='2 Лампы' style='width:300Px;height:100Px' onclick="location.href='leds?lamp=2';">

<br><br><br>
<input type='button' value='Ультрафиолет Включить' style='width:400Px;height:150Px' onclick="location.href='leds?uv=1';">
<input type='button' value='Ультрафиолет потушить' style='width:200Px;height:150Px' onclick="location.href='leds?uv=0';">

<br><br><br>
<input type='button' value='Красный Вкл' style='width:200Px;height:200Px' onclick="location.href='leds?red=1';">
<input type='button' value='Зеленый Вкл' style='width:200Px;height:200Px' onclick="location.href='leds?green=1';">
<input type='button' value='Синий Вкл' style='width:200Px;height:200Px' onclick="location.href='leds?blue=1';">
<br><br><br>
<input type='button' value='Красный Потушить' style='width:200Px;height:200Px' onclick="location.href='leds?red=0';">
<input type='button' value='Зеленый Потушить' style='width:200Px;height:200Px' onclick="location.href='leds?green=0';">
<input type='button' value='Синий Потушить' style='width:200Px;height:200Px' onclick="location.href='leds?blue=0';">

<br><br><br><br><br><br>
<input type='button' value='ОЙ ВСЕ' style='width:600Px;height:300Px' onclick="location.href='leds?lamp=-1';">


<br><br><br><br><br><br><br><br><br>
</body>
</html>
)=====";
