<?php

if (!defined("TEMPERATURE_PATH"))
    define("TEMPERATURE_PATH", "/tmp/temperature/current");

$fp = fopen(TEMPERATURE_PATH, "r"); 
$temperature = fread($fp, filesize(TEMPERATURE_PATH)); 
fclose($fp); 

$temperature = $temperature / 1000.0;
// print $temperature; 

    function tempParts($temp, $index) {
        $parts = explode('.', number_format($temp, 1));
        return $parts[$index];
    }
?>


<html>
<head>
    <title> Current temperature </title>
    <link type="text/css" rel="stylesheet" href="./css/current_temperature.css" />
    <meta name="viewport" content="width=device-width; initial-scale=1.0; maximum-scale=1.0;">
</head>

<body>

<script language="javascript">
    function isMobileDevice() {
        var mobile = (/iphone|ipad|ipod|android|windows\sphone/i.test(navigator.userAgent.toLowerCase()));
        return mobile;
    }

    if(isMobileDevice()) {
        document.write("<link type=\"text\/css\" rel=\"stylesheet\" href=\"./css/mobile.css\" \/>");
    } else {
        document.write("<link type=\"text\/css\" rel=\"stylesheet\" href=\"./css/pc_mac.css\" \/>");
    }
</script>

<div class="container">
    <div class="de">
        <div class="den">
          <div class="dene">
            <div class="denem">
              <div class="deneme">
                <?php print tempParts($temperature, 0); ?><span>.<?php print tempParts($temperature, 1); ?></span><strong>&deg;</strong>
              </div>
            </div>
          </div>
        </div>
    </div>
</div>

<form action="/cgi-bin/temperature.cgi" method="post" class="text_form">
  <table>
    <tr>
      <td>Target: </td>
      <td><input type="number" name="target"></td>
    </tr>
    <tr>
      <td>Tolerance: </td>
      <td><input type="number" name="tolerance"></td>
    </tr>
    <tr>
      <td><input type="submit" value="Submit"></td>
    </tr>
  </table>
</form>

</body>
</html>
