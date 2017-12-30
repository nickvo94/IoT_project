<?php
	
    session_start(); 
    require_once("includes/connection.php"); 

if ($_POST){
	// data duoc nhan tu Request body duoi dang name1=value1&name2=value2
	$temp1=$_POST['temp1'];
	$hum1=$_POST['hum1'];

 	$query = "INSERT INTO tempLog (temperature, humidity) VALUES ('$temp1','$hum1')"; 
  	
   	mysqli_query($con,$query);
	mysqli_close($con);
	
}
?>
