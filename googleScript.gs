//Tomada inteligente
//Projeto Integrador Oitavo Semestre
//Grupo SM2 Polo Sao Jose dos Campos
//Univesp

    var ss = SpreadsheetApp.getActiveSpreadsheet();
    var summarySheet = ss.getSheetByName("ESTADO");
    var dataLoggerSheet = ss.getSheetByName("DADOS");

function doGet(e){
  Logger.log("--- doGet ---");
 
 var tomada1 = "",
     tomada2 = "",
     tomada3 = "",
     corrente = "";
 
  try {
 
    if (e == null){e={}; e.parameters = {tag:"test",value:"-1"};}
 
    tomada1 = e.parameters.tomada1;
    tomada2 = e.parameters.tomada2;
    tomada3 = e.parameters.tomada3;

    corrente = e.parameters.corrente;
 
    save_data(tomada1,tomada2,tomada3,corrente);
 
 var estado1 = summarySheet.getRange("B2").getValue(); 
 var estado2 = summarySheet.getRange("C2").getValue(); 
 var estado3 = summarySheet.getRange("D2").getValue(); 

    
    return ContentService.createTextOutput("Estados: '1:"+estado1+"', '2:"+estado2+"', '3:"+estado3+"'" );
 
  } catch(error) { 
    Logger.log(error);    
    return ContentService.createTextOutput("oops...." + error.message);
  }  
}
 
function save_data(tomada1,tomada2,tomada3,corrente){
  Logger.log("--- save_data ---"); 
 
 
  try {
    var dateTime = new Date();
    var tensao = summarySheet.getRange("A2").getValue(); 
    var potencia = tensao*corrente;
 
     var row = dataLoggerSheet.getLastRow() + 1;
 
    dataLoggerSheet.getRange("A" + row).setValue(dateTime); // dateTime
    dataLoggerSheet.getRange("B" + row).setValue(potencia); // corrente
    dataLoggerSheet.getRange("C" + row).setValue(tomada1); // estado tomada 1
    dataLoggerSheet.getRange("D" + row).setValue(tomada2); // estado tomada 2
    dataLoggerSheet.getRange("E" + row).setValue(tomada3); // estado tomada 3


  }
 
  catch(error) {
    Logger.log(JSON.stringify(error));
  }
 
  Logger.log("--- save_data end---"); 
}

function webAppUrl(){
var scriptID = ScriptApp.getService().getUrl();
summarySheet.getRange("E2").setValue(scriptID);
}
