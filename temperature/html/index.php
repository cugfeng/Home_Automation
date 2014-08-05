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
    <script src="./jquery/jquery-2.1.1.js"></script>
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

<div class="box">
<form id="id_form" class="text_form">
  <table>
    <tr>
      <td>Automode: </td>
      <td><input type="checkbox" name="automode" value="1"></td>
    </tr>
    <tr>
      <td>Target: </td>
      <td><input type="number" name="target"></td>
    </tr>
    <tr>
      <td>Tolerance: </td>
      <td>
      <select id="id_tolerance" name="tolerance">
        <option value="0000">0.0</option>
        <option value="0500">0.5</option>
        <option value="1000">1.0</option>
        <option value="1500">1.5</option>
        <option value="2000">2.0</option>
      </select>
      </td>
    </tr>
    <tr>
      <td><input type="submit" value="Submit"></td>
    </tr>
  </table>
</form>
</div>

<script  language="javascript">
    $("#id_form").submit(function(e) {
        e.preventDefault();
        $.post("./php/submit.php", $("#id_form").serialize(),
            function(data) {
                alert(data);
            });
    });
</script>

</body>
</html>
