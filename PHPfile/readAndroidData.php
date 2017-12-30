<?php

    session_start(); 
    require_once("includes/connection.php"); 

if ($_POST) {
	$decode = json_decode($_REQUEST['request']);	
	$decode = json_decode($_POST['request']);	
	$json = $decode->name;

	$query = "INSERT INTO androidData (name) VALUES ('$json')"; 
  	
   	mysqli_query($con,$query);
	mysqli_close($con);

	header('Content-type: application/json');
	echo json_encode($json);
}

?>