<?php
    session_start(); 
    require_once("includes/connection.php"); 

    $sql="SELECT * FROM androidData ORDER BY id DESC"; 
					  
    $result=mysqli_query($con, $sql); 

    //create an array
    $emparray = array();
    while($row =mysqli_fetch_assoc($result))
    {
        $emparray[] = $row;
    }
    echo json_encode($emparray);

    //close the db connection
    mysqli_close($con);
?>