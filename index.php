<?php
ini_set(\'display_errors\', 1);
require_once(\'TwitterAPIExchange.php\');
 
/** Set access tokens here - see: https://dev.twitter.com/apps/ **/
$settings = array(
    \'oauth_access_token\' => "",
    \'oauth_access_token_secret\' => "",
    \'consumer_key\' => "",
    \'consumer_secret\' => ""
);
 
/** Perform a GET request and echo the response **/
/** Note: Set the GET field BEFORE calling buildOauth(); **/
$url = \'https://api.twitter.com/1.1/search/tweets.json\';
$getfield = \'?q=#UMS_lamp&count=1&include_entities=false\';    // insert search key after "?q=" and before "&"
$requestMethod = \'GET\';
$twitter = new TwitterAPIExchange($settings);
$json = $twitter->setGetfield($getfield)
             ->buildOauth($url, $requestMethod)
             ->performRequest();
//echo $json;
$obj = json_decode($json, true);
$tweet = $obj["statuses"][0]["text"];
echo "Tweet: <tweet>\'$tweet\'</tweet><br>";
$user = $obj["statuses"][0]["user"]["screen_name"];
echo "User: <user>\'@$user\'</user>";
?>
