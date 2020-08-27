<!doctype html>
<html>


<body>
	Hallo
	<?php 
		$name=$_POST["name"];
		$mail=$_POST["email"];
		$number=$_POST["telNr"];
	
		$messageC="Mein Name ist ".$name." meine E-Mail ist ".$mail." Sie erreichen mich unter ".$number." Dankeschön!";
		//echo($name.$mail.$number);
		mail("drothler.edu@gmail.com", $name, $messageC);
		mail($mail, "Bestätigung der Mode-Beratungs-Anfrage", "Sehr geehrte Frau ".$name.",\n Ich habe Ihre Anfrage erhalten und werde sobald wie möglich mit Ihnen in Kontakt treten. Sie können mich auch auf meiner Facebook Seite @example erreichen! \n Ihre Augsburg Modefee, Laura Drothler");
		
	?>
	
</body>
</html>