<?php

$TEMP_SETTING_DIR  = "/tmp/temperature";
$TEMP_SETTING_JSON = "/tmp/temperature/setting.json";

function _post($var) {
    if (!empty($_POST[$var])) {
        return $_POST[$var];
    } else {
        return null;
    }
}

if (!file_exists($TEMP_SETTING_DIR)) {
    mkdir($TEMP_SETTING_DIR, 0755);
    chmod($TEMP_SETTING_DIR, 0777);
}

$automode = _post("automode");
if ($automode == null) {
    $automode = 0;
}

$target = _post("target");
if ($target == null) {
    $target = -1;
}

$tolerance = _post("tolerance");
if ($tolerance == null) {
    $tolerance = -1;
}

$data = "{ \"automode\": $automode, \"target\": $target, \"tolerance\": $tolerance }";
$fp = fopen($TEMP_SETTING_JSON, "w");
fwrite($fp, $data);
fclose($fp);

echo "Automode = $automode\n";
echo "Target = $target\n";
echo "Tolerance = $tolerance\n";

?>
