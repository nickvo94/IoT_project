<?php
//Khởi động session
    session_start(); 
    require_once("includes/connection.php"); 

//SQL DELETE Command: Xóa
$sql  = "DELETE FROM tempLog";

//Query
if (mysqli_query($con, $sql))
{
	// clos connection
	mysqli_close($con);
	// Quay trở lại home page
	header("Location: tempLog.php");
	exit;
}

// clos connection (when query failed!
echo "delete failed!";
mysqli_close($con);
?>