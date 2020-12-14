<?php 
/*
error_reporting(E_ALL);
ini_set("display_errors", 1);
*/

// database settings
$servername = "localhost";
$username = "<<DBUSER>>";
$password = "<<DBPASSWD>>";
$dbname   = "<<DBNAME>>";

// define variables
$sensor = $distance = $temperature = $humidity = $comment = $sql1 = "";


// process input secure
if ($_SERVER["REQUEST_METHOD"] == "GET") {
  $sensor      = test_input($_GET["sensor"] ?? 'none');      
  $distance    = test_input($_GET["distance"] ?? 9999);
  $temperature = test_input($_GET["temperature"] ?? 9999);
  $humidity    = test_input($_GET["humidity"] ?? 9999);
  $comment     = test_input($_GET["comment"] ?? 0);         // runnr from arduino always > 0
} 

function test_input($data) {
  $data = trim($data);
  $data = stripslashes($data);
  $data = htmlspecialchars($data);
  return $data;
}

// insert data when a value is set
if (($distance<>9999)or($temperature<>9999)or($humidity<>9999)) { 
   
    // Create connection
    $conn = new mysqli($servername, $username, $password, $dbname);
    
    // Check connection
    if ($conn->connect_error) {
        die("Connection failed: " . $conn->connect_error);
    }
    
    // build SQL statement
    $sql1 = "INSERT INTO Arduino (sensor";
      if($distance    <> 9999){ $sql1.=", distance";}
      if($temperature <> 9999){ $sql1.=", temperature";}
      if($humidity    <> 9999){ $sql1.=", humidity";}
      if($comment     <>    0){ $sql1.=", comment";}         
    $sql1.=") VALUES ('" . $sensor . "'";
      if($distance    <> 9999){ $sql1.="," . $distance;}
      if($temperature <> 9999){ $sql1.="," . $temperature;}
      if($humidity    <> 9999){ $sql1.="," . $humidity;}
      if($comment     <>    0){ $sql1.=",'" . $comment . "'";}
    $sql1.=")";

    // try to insert and display sent data
    if ($conn->query($sql1) === TRUE) {
      echo 'New record created successfully: ';
      echo 'sensor: ' . $sensor . '    ';
      if($distance    <> 9999){ echo " ** distance: "    . $distance;}
      if($temperature <> 9999){ echo " ** temperature: " . $temperature;}
      if($humidity    <> 9999){ echo " ** humidity: "    . $humidity;}
      if($comment     <>    0){ echo " ** comment: "     . $comment . "'";}

    } else {
      echo 'Error: ' . $sql1 . '    ' . $conn->error . '    ';
    }

    $conn->close();
    
} else {
    echo '-error or no data received-';
}

?>
