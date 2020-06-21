<?php
   
    $CON_CONEXAO = "mysql:host=localhost;dbname=hc4pontozero;charset=utf8";
    $CON_USUARIO = "danilo.eng.mec@hotmail.com";
    $CON_SENHA = "Mestredan3@";
 
    try {
 
        $conexao = new PDO($CON_CONEXAO, $CON_USUARIO, $CON_SENHA);
 
        //echo "Conectado com sucesso";
 
    } catch (PDOException $erro) {
 
        //echo "Erro: " . $erro->getMessage();
        echo "conexao_erro";
 
        exit;
    }
?>