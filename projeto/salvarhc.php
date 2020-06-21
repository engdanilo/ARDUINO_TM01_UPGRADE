<?php
 
    include "conexaohc.php";
 
    $s1_rec = $_GET['s1'];
    $s2_rec = $_GET['s2'];
    $s3_rec = $_GET['s3'];
    $s4_rec = $_GET['s4'];
    $s5_rec = $_GET['s5'];
    $s6_rec = $_GET['s6'];
    $s7_rec = $_GET['s7'];
    $s8_rec = $_GET['s8'];
    $s9_rec = $_GET['s9'];
    $s10_rec = $_GET['s10'];
    $s11_rec = $_GET['s11'];
    $s12_rec = $_GET['s12'];
    $s13_rec = $_GET['s13'];
    $s14_rec = $_GET['s14'];

 
    $SQL_INSERT = "INSERT INTO tb_tm01_variaveis (TEMP, UMID, RPM, COR01, COR02, COR03, COR04, COR05, COR06, COR07, COR08, COR09, OPER, MANUT)";
    $SQL_INSERT .= " VALUES (:S1, :S2, :S3, :S4, :S5, :S6, :S7, :S8, :S9, :S10, :S11, :S12, :S13, :S14)";

    $stmt = $conexao->prepare($SQL_INSERT);
 
    $stmt->bindParam(":S1", $s1_rec);
    $stmt->bindParam(":S2", $s2_rec);
    $stmt->bindParam(":S3", $s3_rec);
    $stmt->bindParam(":S4", $s4_rec);
    $stmt->bindParam(":S5", $s5_rec);
    $stmt->bindParam(":S6", $s6_rec);
    $stmt->bindParam(":S7", $s7_rec);
    $stmt->bindParam(":S8", $s8_rec);
    $stmt->bindParam(":S9", $s9_rec);
    $stmt->bindParam(":S10", $s10_rec);
    $stmt->bindParam(":S11", $s11_rec);
    $stmt->bindParam(":S12", $s12_rec);
    $stmt->bindParam(":S13", $s13_rec);
    $stmt->bindParam(":S14", $s14_rec);

 
    if($stmt->execute()) {
 
        echo "insert_ok";
       
    } else {
 
        echo "insert_erro";
    }
?>