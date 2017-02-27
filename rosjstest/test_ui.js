// Init ROS
var ros=new ROSLIB.Ros({
	url : "ws://localhost:9090"
});

ros.on("connection", function() {
	console.log("Connected");
});
ros.on("error", function(error) {
	console.log("error", error);
});
ros.on("close", function() {
	console.log("closed");
});

// Publish a Topic
var talker = new ROSLIB.Topic({
	ros : ros, 
	name : "talker", 
	messageType : "std_msgs/String"
});

// Subscribe a Topic
var listener = new ROSLIB.Topic({
	ros : ros, 
	name : "listener", 
	messageType : "std_msgs/String"
});
listener.subscribe(function(msg) {
	console.log("I heard : " + msg.data);
	document.Subscribed.Listener.value = msg.data;
});

function Cat() {
	var msg = new ROSLIB.Message({
		data : "Meow-Meow!"
	});
	talker.publish(msg);
}
function Dog() {
	var msg = new ROSLIB.Message({
		data : "Bow-wow!"
	});
	talker.publish(msg);
}
