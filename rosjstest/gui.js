var ros = new ROSLIB.Ros({
    url: "ws://localhost:9090"
});

ros.on("connection" function(){
    console.log("Connected");
});
ros.on("error",function(error){
    console.log("error",error);
});
ros.on("close",function(){
    console.log("closed");
});

var talker = new ROSLIB.Topic({
    ros : ros,
    name : "talker",
    messageType : "std_msgs/String"
});

var listener = new ROSLIB.Topic({
    ros : ros,
    name : "listener",
    messageType : "std_msgs/String"
});

//
function start(){
    var msg = new ROSLIB.Message({
	state : "start"
    });
    talker.publish(msg);
}


//
function stop(){
    var msg = new ROSLIB.Message({
	state : "stop"
    });
    talker.publish(msg);
}

//
function pause(){
    var msg = new ROSLIB.Message({
	state : "pause"
    });
    talker.publish(msg);
}

listener.subscribe(function(msg){
    console.log("I heard:" + msg.data);
    document.Subscribed.Listener.value = msg.data;
});

/*
//
function print_co2(){
    var co2 = new ROSLIB.Message({
});
listener.subscirbe(
    document.writeln("CO2:" + co2);
}

//
function print_temperature(){
    var d = new Number();
    var tem1 = d.;
    var tem2 = d.;
    document.writeln("Temperature:" + tem1 + "~" + tem2);
}

//
function change_temperature(){


}

//
function victimignore(){

}

//
function victimcofirm(){

}

//
function change_btn(find){
    if(find == true){
	document.victim_confirm.disabled = true;
    }else{
	document.victim_confirm.disabled = false;
    }
}

//
function vfind(){
    var d = new Number();
    var vfind = d.;
	change_btn(vfind);
}

//
function sound(find){
    if(find == true){
	document.getElementById("gui_snd.mp3").play();
    }
}

*/
