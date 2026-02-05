/**
 * Add gobals here
 */
var seconds 	= null;
var otaTimerVar =  null;
var wifiConnectInterval = null;

/**
 * Initialize functions here.
 */
$(document).ready(function(){
	getUpdateStatus();
	// startLocalTimeInterval();
    getConnectInfo();
	$("#connect_wifi").on("click", function(){
		checkCredentials();
	}); 
	$("#disconnect_wifi").on("click", function(){
		disconnectWifi();
	}); 
});

// OTA FIRMWARE UPDATES //

/**
 * Gets file name and size for display on the web page.
 */        
function getFileInfo() 
{
    var x = document.getElementById("selected_file");
    var file = x.files[0];

    document.getElementById("file_info").innerHTML = "<h4>File: " + file.name + "<br>" + "Size: " + file.size + " bytes</h4>";
}

/**
 * Handles the firmware update.
 */
function updateFirmware() 
{
    // Form Data
    var formData = new FormData();
    var fileSelect = document.getElementById("selected_file");
    
    if (fileSelect.files && fileSelect.files.length == 1) 
	{
        var file = fileSelect.files[0];
        formData.set("file", file, file.name);
        document.getElementById("ota_update_status").innerHTML = "Uploading " + file.name + ", Firmware Update in Progress...";

        // Http Request
        var request = new XMLHttpRequest();

        request.upload.addEventListener("progress", updateProgress);
        request.open('POST', "/OTAupdate");
        request.responseType = "blob";
        request.send(formData);
    } 
	else 
	{
        window.alert('Select A File First')
    }
}

/**
 * Progress on transfers from the server to the client (downloads).
 */
function updateProgress(oEvent) 
{
    if (oEvent.lengthComputable) 
	{
        getUpdateStatus();
    } 
	else 
	{
        window.alert('total size is unknown')
    }
}

/**
 * Posts the firmware udpate status.
 */
function getUpdateStatus() 
{
    var xhr = new XMLHttpRequest();
    var requestURL = "/OTAstatus";
    xhr.open('POST', requestURL, false);
    xhr.send('ota_update_status');

    if (xhr.readyState == 4 && xhr.status == 200) 
	{		
        var response = JSON.parse(xhr.responseText);
						
	 	document.getElementById("latest_firmware").innerHTML = response.compile_date + " - " + response.compile_time

		// If flashing was complete it will return a 1, else -1
		// A return of 0 is just for information on the Latest Firmware request
        if (response.ota_update_status == 1) 
		{
    		// Set the countdown timer time
            seconds = 10;
            // Start the countdown timer
            otaRebootTimer();
        } 
        else if (response.ota_update_status == -1)
		{
            document.getElementById("ota_update_status").innerHTML = "!!! Upload Error !!!";
        }
    }
}

/**
 * Displays the reboot countdown.
 */
function otaRebootTimer() 
{	
    document.getElementById("ota_update_status").innerHTML = "OTA Firmware Update Complete. This page will close shortly, Rebooting in: " + seconds;

    if (--seconds == 0) 
	{
        clearTimeout(otaTimerVar);
        window.location.reload();
    } 
	else 
	{
        otaTimerVar = setTimeout(otaRebootTimer, 1000);
    }
}

/**************************
**		CONNECTING  	 **
**************************/

/**
 * Clears the connection status interval.
 */
function stopWifiConnectStatusInterval()
{
	if (wifiConnectInterval != null)
	{
		clearInterval(wifiConnectInterval);
		wifiConnectInterval = null;
	}
}

/**
 * Gets the WiFi connection status.
 */
function getWifiConnectStatus()
{
	var xhr = new XMLHttpRequest();
	var requestURL = "/wifiConnectStatus";
	xhr.open('POST', requestURL, false);
	xhr.send('wifi_connect_status');
	
    // 4 = sent
	if (xhr.readyState == 4 && xhr.status == 200)
	{
		var response = JSON.parse(xhr.responseText);
		
		document.getElementById("wifi_connect_status").innerHTML = "Connecting...";
		
		if (response.wifi_connect_status_json == 2)
		{
			document.getElementById("wifi_connect_status").innerHTML = "<h4 class='rd'>Failed to Connect. Please check your AP credentials and compatibility</h4>";
			stopWifiConnectStatusInterval();
		}
		else if (response.wifi_connect_status_json == 3)
		{
			document.getElementById("wifi_connect_status").innerHTML = "<h4 class='gr'>Connection Success!</h4>";
			stopWifiConnectStatusInterval();
            getConnectInfo();
		}
	}
}

/**
 * Starts the interval for checking the connection status.
 */
function startWifiConnectStatusInterval()
{
	wifiConnectInterval = setInterval(getWifiConnectStatus, 2800);
}

/**
 * Connect WiFi function called using the SSID and password entered into the text fields.
 */
function connectWifi()
{
	// Get the SSID and password
	selectedSSID = $("#connect_ssid").val();
	pwd = $("#connect_pass").val();
	
	$.ajax({
		url: '/wifiConnect.json',
		dataType: 'json',
		method: 'POST',
		cache: false,
		data: JSON.stringify({ c_ssid: selectedSSID, c_pwd: pwd}),
	});
	
	startWifiConnectStatusInterval();
}

/**
 * Checks credentials on connect_wifi button click.
 */
function checkCredentials()
{
	errorList = "";
	credsOk = true;
	
	selectedSSID = $("#connect_ssid").val();
	pwd = $("#connect_pass").val();
	
	if (selectedSSID == "")
	{
		errorList += "<h4 class='rd'>SSID cannot be empty!</h4>";
		credsOk = false;
	}
	if (pwd == "")
	{
		errorList += "<h4 class='rd'>Password cannot be empty!</h4>";
		credsOk = false;
	}
	
	if (credsOk == false)
	{
		$("#wifi_connect_credentials_errors").html(errorList);
	}
	else
	{
		$("#wifi_connect_credentials_errors").html("");
		connectWifi();    
	}
}

/**
 * Shows the WiFi password if the box is checked.
 */
function showPassword()
{
	var x = document.getElementById("connect_pass");
	if (x.type === "password")
	{
		x.type = "text";
	}
	else
	{
		x.type = "password";
	}
}

/**
 * Gets the connection information for displaying on the web page
 */
function getConnectInfo()
{
    console.log("should have displayed something here dude");
    $.getJSON('/wifiConnectInfo.json', function(data)
    {
        $("#connected_ap_label").html("Connected to: "); 
        $("#connected_ap").text(data["ap"]);
        
        $("#ip_address_label").html("IP Adress: "); 
        $("#wifi_connect_ip").text(data["ip"]);
        
        $("#netmask_label").html("Netmask: "); 
        $("#wifi_connect_netmask").text(data["netmask"]);
        
        $("#gateway_label").html("Gateway: "); 
        $("#wifi_connect_gateway").text(data["gateway"]);

        document.getElementById('disconnect_wifi').style.display = 'block';         
    });
}

/**
 * Disconnect WiFi once the disconnect button is pressed, and reloads the web page.
 */
function disconnectWifi()
{
	$.ajax({
		url: '/wifiDisconnect.json',
		dataType: 'json',
		method: 'DELETE',
		cache: false,
		data: { 'timestamp': Date.now() }
	});
	// Update the web page
	setTimeout("location.reload(true);", 2000);
}

/**
 * Sets the interval for displaying local time.
 */
function startLocalTimeInterval()
{
	setInterval(getLocalTime, 10000);
}

/**
 * Gets the local time.
 * @note connect the ESP32 to the internet and the time will be updated.
 */
function getLocalTime()
{
	$.getJSON('localTime.json', function(data)
	{
		$("#local_time").text(data["time"]);
	});
}

function acionarBomba() {
	var bombaStatus = false; // Variável receberá o status atual da bomba do backend
	var button = document.getElementById("btn-acionar-bomba");
	var label = document.getElementById("status-bomba-atual");

	if(button.innerText == "Acionar Bomba"){
		bombaStatus = false;
		alert("Bomba acionada!");
	}
	if(button.innerText == "Desacionar Bomba"){
		bombaStatus = true;
		alert("Bomba desacionada!");
	}

	if (bombaStatus == false) {
		button.style.backgroundColor = "#83110C";
		button.innerText = "Desacionar Bomba";
		label.style.color = "#04AA6D";
		label.innerText = "Ligado";
		bombaStatus = true;
	} else {
		button.style.backgroundColor = "#04AA6D";
		button.innerText = "Acionar Bomba";
		label.style.color = "#83110C";
		label.innerText = "Desligado";
		bombaStatus = false;
	}
}

const cidade = document.getElementById("cidade");
cidade.addEventListener("load", mudarCidade());

const previsao = document.getElementById("complementos-previsao");
previsao.addEventListener("load", atualizaTempo());


//modificar para evento de atualização dos dados dos sensores
const dadosSensores = document.getElementById("dados-status");
dadosSensores.addEventListener("load", atualizaDadosSensores());

function mudarCidade() {
	var localizacao = "Uberlândia"; //Colocar variável que recebe o nome da cidade do backend
	cidade.innerText = localizacao;
}

function atualizaTempo(){
	var temperatura = document.getElementById("temperatura-atual");
	var max = document.getElementById("temp-max-dia");
	var min = document.getElementById("temp-min-dia");
	var vento = document.getElementById("vento-dia");
	var umidade = document.getElementById("umidade-dia");
	var pressao = document.getElementById("pressao-dia");
	var precipitacao = document.getElementById("precipitacao-dia");

	atualizaAnimacaoTempo();
	var dados = [25, 30, 20, 15, 60, 1013, 5]; //Colocar variáveis que recebem os dados do backend

	temperatura.innerText = dados[0] + " °C"; 
	max.innerText = dados[1] + " °C"; 
	min.innerText = dados[2] + " °C"; 
	vento.innerText = dados[3] + " km/h"; 
	umidade.innerText = dados[4] + " %"; 
	pressao.innerText = dados[5] + " hPa"; 
	precipitacao.innerText = dados[6] + " mm"; 


}

function atualizaAnimacaoTempo(){
	var tempo = document.getElementById("tempo-atual");

	//condições do tempo:
	//1-ensolarado 2-encoberto 3-noite 4-nublado-dia 5-nublado-noite
	//6-pouca-chuva-sol 7-muita-chuva-sol 8-garoa 9-pouca-chuva 10-chuva-moderada
	//11-muita-chuva 12-tempestade 13-geada
	var tempobackend = 1; //Colocar variável que recebe condição do tempo do backend
	tempo.src = "tempo/animated/" + tempobackend +".svg";
}

function atualizaDadosSensores(){
	var umidadeSolo = document.getElementById("umidade-solo");
	var temperaturaSolo = document.getElementById("temperatura-solo");
	var pressaoSolo = document.getElementById("pressao-solo");

	var dadosSolo = [45, 22, 1010]; //Colocar variáveis que recebem os dados do backend

	umidadeSolo.innerText = dadosSolo[0] + " %"; 
	temperaturaSolo.innerText = dadosSolo[1] + " °C"; 
	pressaoSolo.innerText = dadosSolo[2] + " hPa"; 
}