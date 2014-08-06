<?php

$TEMPERATURE_PATH = "/tmp/temperature/current";

$fp = fopen($TEMPERATURE_PATH, "r");
$temperature = fread($fp, filesize($TEMPERATURE_PATH));
fclose($fp);

echo $temperature;

?>
