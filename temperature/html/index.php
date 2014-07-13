<?php

if (!defined("TEMPERATURE_PATH"))
	define("TEMPERATURE_PATH", "/tmp/current_temperature"); 

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

</body>
</html>
