// This BDCCArrow is a VML/SVG overlay that shows a coloured arrow on the map
// The arrow line is 10 pixels long and 2 pixels thick. The head is added
// using VML or SVG 
//
// This is just a starter demo, useful for showing a GPS track etc.
//
// Only event so far is 'click' 
//
// Bill Chadwick 2007

var arrowMarkerCounter;//unique id counter for SVG arrow head markers

function BDCCArrow(point, rotation, color, opacity, tooltip) {

  this.point_ = point;
  this.rotation_ = rotation;
  var r = rotation + 90;//compass to math
  this.dx_ = 20*Math.cos(r*Math.PI/180);//other end of arrow line to point
  this.dy_ = 20*Math.sin(r*Math.PI/180);
  this.color_ = color || "#888888";
  this.opacity_ = opacity || 0.5;
  this.tooltip_ = tooltip;
  
  if (arrowMarkerCounter == null)
	arrowMarkerCounter = 0;
  else
    arrowMarkerCounter += 1;  
  this.svgId_ = "BDCCArrow" + arrowMarkerCounter.toString();

}
BDCCArrow.prototype = new google.maps.Overlay();

BDCCArrow.prototype.getPoint = function(){
    return this.point_;
}

BDCCArrow.prototype.getTooltip = function(){
    return this.tooltip_;
}

BDCCArrow.prototype.clicked = function(){
    GEvent.trigger(this,"click");
}

// Creates the DIV representing this arrow.
BDCCArrow.prototype.initialize = function(map) {

  var div = document.createElement("DIV");
  div.title = this.tooltip_;
  div.style.cursor = "help";

  // Arrow is similar to a marker, so add to plane just below marker pane
  map.getPane(G_MAP_MARKER_SHADOW_PANE).appendChild(div);

  //save for later
  this.map_ = map;
  this.div_ = div;
  
  //set up arrow invariants
  if(navigator.userAgent.indexOf("MSIE") != -1){

	var l = document.createElement("v:line");
	l.strokeweight = "3px";
	l.strokecolor = this.color_;
	var s = document.createElement("v:stroke");
	s.opacity = this.opacity_;
	if(this.rotation_ >= 0)
		s.startarrow="classic";// or "block", "open" etc see VML spec
	l.appendChild(s);
	this.div_.appendChild(l);
	this.vmlLine_ = l;
  }
  else{

    // make a 40x40 pixel space centered on the arrow 
	var svgNS = "http://www.w3.org/2000/svg";
	var svgRoot = document.createElementNS(svgNS, "svg");
	svgRoot.setAttribute("width", 40);
	svgRoot.setAttribute("height", 40);
	svgRoot.setAttribute("stroke",this.color_);
	svgRoot.setAttribute("fill",this.color_);
	svgRoot.setAttribute("stroke-opacity",this.opacity_);
	svgRoot.setAttribute("fill-opacity",this.opacity_);
	this.div_.appendChild(svgRoot);

	var svgNode = document.createElementNS(svgNS, "line");
	svgNode.setAttribute("stroke-width",3);
	svgNode.setAttribute("x1",20);
	svgNode.setAttribute("y1",20);
	svgNode.setAttribute("x2",20+this.dx_);
	svgNode.setAttribute("y2",20+this.dy_);

    //make a solid arrow head, can't share these, as in SVG1.1 they can't get color from the referencing object, only their parent
    //a bit more involved than the VML
	if(this.rotation_ >= 0){
      	var svgM = document.createElementNS(svgNS, "marker");
      	svgM.id=this.svgId_;
      	svgM.setAttribute("viewBox","0 0 10 10");
      	svgM.setAttribute("refX",0);
      	svgM.setAttribute("refY",5); 
      	svgM.setAttribute("markerWidth",4);
      	svgM.setAttribute("markerHeight",3);
      	svgM.setAttribute("orient","auto");
      	var svgPath = document.createElementNS(svgNS, "path");//could share this with 'def' and 'use' but hardly worth it 
		svgPath.setAttribute("d","M 10 0 L 0 5 L 10 10 z");
		svgM.appendChild(svgPath);
		svgRoot.appendChild(svgM);
		svgNode.setAttribute("marker-start","url(#" + this.svgId_ + ")");
    }

	svgRoot.appendChild(svgNode);
	this.svgRoot_ = svgRoot;

  }
  
  //set up click handler
  var cb = GEvent.callback(this,this.clicked);
  this.clickH_ = GEvent.addDomListener(this.div_,"click",function(event){cb()});
  
}

// Remove the main DIV from the map pane
BDCCArrow.prototype.remove = function() {
  GEvent.removeListener(this.clickH_);
  this.div_.parentNode.removeChild(this.div_);
}

// Copy our data to a new BDCCArrow
BDCCArrow.prototype.copy = function() {
  return new BDCCArrow(this.point_, this.rotation_, this.color_, this.opacity_, this.tooltip_);
}

// Redraw the arrow based on the current projection and zoom level

BDCCArrow.prototype.redraw = function(force) {

  // We only need to redraw if the coordinate system has changed
  if (!force) return;

  // Calculate the DIV coordinates of the ref point of our arrow
  var p = this.map_.fromLatLngToDivPixel(this.point_);
  var x2 = p.x + this.dx_; 
  var y2 = p.y + this.dy_; 

  if(navigator.userAgent.indexOf("MSIE") != -1){
	this.vmlLine_.from = p.x + "px, " + p.y + "px";
	this.vmlLine_.to = x2 + "px, " + y2 + "px";
  }
  else{
	this.svgRoot_.setAttribute("style", "position:absolute; top:"+ (p.y-20) + "px; left:" + (p.x-20) + "px");
  }

}





