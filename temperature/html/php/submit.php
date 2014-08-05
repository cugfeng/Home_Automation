<?php

$TEMP_SETTING_DIR       = "/tmp/temperature";
$TEMP_SETTING_CURRENT   = "/tmp/temperature/current";
$TEMP_SETTING_AUTOMODE  = "/tmp/temperature/automode";
$TEMP_SETTING_TARGET    = "/tmp/temperature/target";
$TEMP_SETTING_TOLERANCE = "/tmp/temperature/tolerance";
$TEMP_SETTING_EXIT      = "/tmp/temperature/exit";

function _post($var) {
    if (!empty($_POST[$var]))
        return $_POST[$var];
    else
        return null;
}

if (!file_exists($TEMP_SETTING_DIR)) {
    mkdir($TEMP_SETTING_DIR, 0755);
    chmod($TEMP_SETTING_DIR, 0777);
}

$automode = _post("automode");
if ($automode == null)
    $automode = "0";
else
    $automode = "1";

$target = _post("target");
if ($target == null)
    $target = -1;
else
    $target = (string)((int)$target * 1000);

$tolerance = _post("tolerance");

$fp = fopen($TEMP_SETTING_AUTOMODE, "w");
fwrite($fp, $automode);
fclose($fp);

$fp = fopen($TEMP_SETTING_TARGET, "w");
fwrite($fp, $target);
fclose($fp);

$fp = fopen($TEMP_SETTING_TOLERANCE, "w");
fwrite($fp, $tolerance);
fclose($fp);

echo "Automode = $automode\n";
echo "Target = $target\n";
echo "Tolerance = $tolerance\n";

?>
