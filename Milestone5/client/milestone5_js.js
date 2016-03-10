//http://thecodeplayer.com/walkthrough/html5-game-tutorial-make-a-snake-game-using-html5-canvas-jquery

//Variables
var Server;
var connected = false;
var canvas;
var GH = 0; //Width of game in single units
var GW = 0; //Height of game in single units
var w; //Width of game in pixels
var h; //Height of game in pixels
var cw = 10; //Cell Width
var snake = new Array(2);
var latencyTimes = 0;
var receivedMsgs = 0;
var avgLatency;
var receivedBoard = 0;
var playerID = 0; //ASK ARIELLE TO SEND THIS WITH THE SERVER.
var P1Dir = "D";
var P2Dir = "A";
var food;


//Log messages to the log output textarea
function log( text ) {
		$log = $('#log');
		//Add text to log
		$log.append(($log.val()?"\n":'')+text);
		//Autoscroll
		$log[0].scrollTop = $log[0].scrollHeight - $log[0].clientHeight;
}

	
//connects to server at IP address and PORT supplied by user.
function connect(){
	log('Connecting...');

	Server = new FancyWebSocket('ws://' + document.getElementById('ip').value + ':' + document.getElementById('port').value);
	
	//Let the user know we're connected
	Server.bind('open', function () {
		log('Connected.');
		document.getElementById("cntBtn").disabled = true; // Disables connect botton
		document.getElementById("gameArea").style.visibility = 'visible'; //makes gameArea Visible
		document.getElementById("user_input").style.visibility = 'hidden'; //hides connection form
		send("NewPlayer:" + document.getElementById("playerName").value);
		//Sends username to server
		connected = true;
		init(); //calls gameboard
	});
	//Disconnection occurred.
	Server.bind('close', function( data ) {
		document.getElementById("cntBtn").disabled = false; //Re-enables connect button
		document.getElementById("user_input").style.visibility = 'visible';// Re-enables connection form
		log( "Disconnected." );
	});
	Server.bind('message', function( payload ) {
		processPayload(payload);
	});

	Server.connect();
}

function send_key( key ) {
	Server.send('key', key);
}

//Sends message to the server, attaches send time for latency logging
function send( text ) {
	var to_send = "t:".concat(Date.now());
	to_send = to_send.concat(":");
	to_send = to_send.concat(text);
	Server.send( 'message',  to_send);
}

//connects to server at IP address and PORT supplied by user.
function connect(){
	log('Connecting...');

	Server = new FancyWebSocket('ws://' + document.getElementById('ip').value + ':' + document.getElementById('port').value);
	
	//Let the user know we're connected
	Server.bind('open', function () {
		log('Connected.');
		document.getElementById("cntBtn").disabled = true; // Disables connect botton
		document.getElementById("gameArea").style.visibility = 'visible'; //makes gameArea Visible
		document.getElementById("user_input").style.visibility = 'hidden'; //hides connection form
		send("NewPlayer:" + document.getElementById("playerName").value);
		//Sends username to server
		connected = true;
		init(); //calls gameboard
	});
	//Disconnection occurred.
	Server.bind('close', function( data ) {
		document.getElementById("cntBtn").disabled = false; //Re-enables connect button
		document.getElementById("user_input").style.visibility = 'visible';// Re-enables connection form
		log( "Disconnected." );
	});
	Server.bind('message', function( payload ) {
		processPayload(payload);
	});

	Server.connect();
}

 function processPayload(payload){
	//payload is received as one long string, slices and processes based off of prefix
	var timeNow = Date.now();
	var commands = []
	commands = payload.split("_");
	for(var i=0; i < commands.length; i++){
		if(commands[i].slice(0,2) == "t0"){
			calcLatency(commands[i], timeNow);
			document.getElementById("avg_latency").innerHTML = "Average Latency: ".concat(String(avgLatency).concat(" seconds."));
			//send("AVGL:".concat(String(avgLatency)));
		}
		else if(commands[i].slice(0,1) == "1") {
			document.getElementById("p1score").innerHTML = commands[i].slice(2);
			}
		else if (commands[i].slice(0,1) == "2") {
			document.getElementById("p2score").innerHTML = commands[i].slice(2);
			}
		else if(commands[i].slice(0,2) == "GW"){
			GW = parseInt(commands[i].slice(3));
			w = GW * 10;
		}
		else if(commands[i].slice(0,2) == "GH"){
			GH = parseInt(commands[i].slice(3));
			h = GH * 10; 
			}
		else if(commands[i].slice(0,2) == "GB"){
			getBoard(commands[i].slice(3));
			
		} else if(commands[i].slice(0,2) == "ID"){
			playerID = parseInt(commands[i].slice(3));
			log(playerID);
		} else if (commands[i].slice(0,4) == "MOVE"){
			var directions = commands[i].slice(5);
			log("MOVE: " + directions);
			if(playerID == 0){	
				P2Dir = directions[1];
			} else {
				P1Dir = directions[0];
			}
		} else if(commands[i].slice(0,2) == "RT"){
			P1Dir = "D";
			P2Dir = "A";
		}
		else {log(commands[i]);}
	}
}
function calcLatency(latencyText, t3){
	//calculates latency given the text supplied and T3
	var times = latencyText.split(";");
	times[0] = times[0].slice(3);
	times[1] = times[1].slice(3);
	times[2] = times[2].slice(3);
	var t0 = parseInt(times[0]);
	var t1 = parseInt(times[1]);
	var t2 = parseInt(times[2]);
	latencyTimes += ((t3-t0) - (t2 - t1))/2;
	receivedMsgs += 1;
	avgLatency = latencyTimes/(receivedMsgs * 1000);
	
}

function getBoard(payload){
	//takes gameboard information sent from server and translates it to snakes
	snake[0] = [];
	snake[1] = [];
	var gameBoard = new Array(GH);
	for(var i = 0; i < GH; i++){ gameBoard[i] = new Array(GW);}
		var j = 0;
		for (var i = 0; i < payload.length; i++) {
		  if (i != 0 && i % GW === 0) {
			j++;
		  }
		  var letter = payload[i];
		  var loc = i % GW;
		  gameBoard[j][loc] = letter;
		  if (letter == "1" || letter == "2") {
			  num =parseInt(letter) 
			snake[num-1].push({
			  x: loc,
			  y: j
			});
		  }
		  else if (letter =="F"){
			  food = {x: loc, y: j};
			}	
		}
	receivedBoard = 1;
}

function init()
{
	if(connected) { 
		d = "right"; //default direction
		//Lets move the snake now using a timer which will trigger the paint function
		//every 60ms
		latencyTimes = 0;
		receivedMsgs = 0;
		if(typeof game_loop != "undefined") clearInterval(game_loop);
		receivedBoard = 0;
		game_loop = setInterval(paint, 501);
	} else { setTimeout(init, 4000);}
}

function moveSnakes(){
	var tempdir;
	for(var i =0; i < 2; i++){
		var nx = snake[i][0].x;
			var ny = snake[i][0].y;

			//These were the position of the head cell.
			//We will increment it to get the new head position
			//Lets add proper direction based movement now
			if(i == 0){tempdir = P1Dir;} 
			else{ tempdir = P2Dir;}
			if(tempdir == "D") nx++;
			else if(tempdir == "A") nx--;
			else if(tempdir == "W") ny--;
			else if(tempdir == "S") ny++;
			tempdir = "";

			if(nx == -1 || nx == w/cw || ny == -1 || ny == h/cw || check_collision(nx, ny, snake[i]))
			{
				//setTimeout(game_over, 3000); 
				return;
			}

			//Lets write the code to make the snake eat the food
			//The logic is simple
			//If the new head position matches with that of the food,
			//Create a new head instead of moving the tail
			if(nx == food.x && ny == food.y)
			{
				var tail = {x: nx, y: ny};
			}
			else
			{
				var tail = snake[i].pop(); //pops out the last cell
				tail.x = nx; tail.y = ny;
			}

			//The snake can now eat the food.
			
			snake[i].unshift(tail); //puts back the tail as the first cell
	}
	
}

function check_collision(x, y, array)
{
	//This function will check if the provided x/y coordinates exist
	//in an array of cells or not
	for(var i = 0; i < array.length; i++)
	{
		if(array[i].x == x && array[i].y == y)
		 return true;
	}
	return false;
}

function paint_cell(x, y, color)
{
	ctx.fillStyle = color;
	ctx.fillRect(x*cw, y*cw, cw, cw);
	ctx.strokeStyle = "white";
	ctx.strokeRect(x*cw, y*cw, cw, cw);
}

function paint()
{
	if(playerID == 0){
		
		toSend = "COMMAND:".concat(P1Dir);
		send(toSend);
	}
	else { 
		toSend = "COMMAND:".concat(P2Dir);
		send(toSend);
	}
	//To avoid the snake trail we need to paint the BG on every frame
	//Lets paint the canvas now
	

	
	ctx.fillStyle = "white";
	ctx.fillRect(0, 0, w, h);
	ctx.strokeStyle = "black";
	ctx.strokeRect(0, 0, w, h);
	
			
	//paints food to canvas
	paint_cell(food.x, food.y, "green");
	moveSnakes();
	//if(receivedBoard == 0){
		
	//}
	for(var i = 0; i < snake[0].length; i++)
	{
		var c = snake[0][i];
		//Lets paint 10px wide cells
		paint_cell(c.x, c.y, "red");
	}
	for(var i = 0; i < snake[1].length; i++)
	{
		var c = snake[1][i];
		//Lets paint 10px wide cells
		paint_cell(c.x, c.y, "blue");
	}
	
	receivedBoard = 0;

	
}

$(document).keydown(function(e){
	//receives key pressed, identifies direction, sends movement of snake. 
	var key = e.which;
	var direction;
	if(playerID == 0) 
	{
		direction = P1Dir;
	} 
	else {
		direction = P2Dir;
	}
	if(key == "37" && direction != "D") direction = "A";
	else if(key == "38" && direction != "S") direction = "W";
	else if(key == "39" && direction != "A") direction = "D";
	else if(key == "40" && direction != "W") direction = "S";
	else if(key == "65" && direction != "D") direction = "A";
	else if(key == "87" && direction != "S") direction = "W";
	else if(key == "68" && direction != "A") direction = "D";
	else if(key == "83" && direction != "W") direction = "S";
	if(["W","A","S","D"].indexOf(direction) >= 0){
		//log(direction);

		if(playerID == 0) 
		{
			P1Dir = direction;
		} 
		else {
			P2Dir = direction;
		}
		
		
	
	}
})

$(document).ready(function(){

	canvas = $("#canvas")[0];
	ctx = canvas.getContext("2d");
})
	