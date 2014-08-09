<?php

$TEMP_SETTING_DIR  = "/tmp/temperature";
$TEMP_SETTING_JSON = "/tmp/temperature/setting.json";

function _post($var) {
    if (isset($_POST[$var])) {
        return $_POST[$var];
    } else {
        return null;
    }
}

if (!file_exists($TEMP_SETTING_DIR)) {
    mkdir($TEMP_SETTING_DIR, 0755);
    chmod($TEMP_SETTING_DIR, 0777);
}

$automode    = _post("automode");
$from_hour   = _post("from_hour");
$from_minute = _post("from_minute");
$from_ampm   = _post("from_ampm");
$to_hour     = _post("to_hour");
$to_minute   = _post("to_minute");
$to_ampm     = _post("to_ampm");
$target      = _post("target");
$tolerance   = _post("tolerance");

if ($automode == 1) {
$data = "{
    \"automode\": $automode,
    \"from_hour\": $from_hour,
    \"from_minute\": $from_minute,
    \"from_ampm\": \"$from_ampm\",
    \"to_hour\": $to_hour,
    \"to_minute\": $to_minute,
    \"to_ampm\": \"$to_ampm\",
    \"target\": $target,
    \"tolerance\": $tolerance
}";
} else {
    $data = "{ \"automode\": $automode }";
}

$fp = fopen($TEMP_SETTING_JSON, "w");
fwrite($fp, $data);
fclose($fp);

echo "$data"

?>
