<?php

    session_start(); 
    require_once("includes/connection.php");

if(isset($_POST["1"])) {
    $query1 = "INSERT INTO androidData (name) VALUES ('1')";
    mysqli_query($con,$query1);
    header("Location: http://users.metropolia.fi/~hieuv/tempLog.php");
    exit;
    }
if(isset($_POST["0"])) {
    $query2 = "INSERT INTO androidData (name) VALUES ('0')";
    mysqli_query($con,$query2);
    header("Location: http://users.metropolia.fi/~hieuv/tempLog.php");
    exit;    	
    }

mysqli_close($con);

?>