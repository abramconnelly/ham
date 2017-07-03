var str = "c rgr n10, e20, seed 0\np edge 10 20\ne 9 4\ne 4 9\ne 8 10\ne 10 8\ne 2 4\ne 4 2\ne 8 3\ne 3 8\ne 6 7\ne 7 6\ne 4 10\ne 10 4\ne 2 7\ne 7 2\ne 1 3\ne 3 1\ne 2 9\ne 9 2\ne 2 5\ne 5 2\ne 2 10\ne 10 2\ne 3 6\ne 6 3\ne 9 7\ne 7 9\ne 3 7\ne 7 3\ne 6 5\ne 5 6\ne 10 3\ne 3 10\ne 8 6\ne 6 8\ne 8 5\ne 5 8\ne 9 3\ne 3 9\n";

var str2= "c rgr n10, e20, seed 0\np edge 10 20\ne 9 4\ne 8 10\ne 2 4\ne 8 3\ne 6 7\ne 4 10\ne 10 8\ne 2 7\ne 1 3\ne 2 9\ne 2 5\ne 2 10\ne 3 6\ne 9 7\ne 3 7\ne 6 5\ne 10 3\ne 8 6\ne 8 5\ne 9 3\n";

var str3="c rgr n10, e20, seed 0\np edge 10 20\ne 4 9\ne 8 10\ne 2 4\ne 3 8\ne 5 6\ne 4 7\ne 6 10\ne 7 10\ne 2 8\ne 1 7\ne 2 3\ne 2 9\ne 2 5\ne 2 10\ne 3 6\ne 7 9\ne 3 7\ne 3 10\ne 6 8\n";


var old_config = {
  styles: {
    node: { texture: "images/node.png", label: { hideSize: 16 } },
    edge: { arrow: { texture: "images/arrow.png" } }
	}
};


var config = {
  styles: {
    edgeHighlight:{color:"rgb(255,128,128)" },
    background: { color: "rgb(255, 255, 255)" },
    node: {
      minSize: 6,
      maxSize: 16,
      color: "rgb(255, 0, 0)",
      texture: "images/circle.png",
      label: { hideSize: 16, color: "rgb(120, 120, 120)" }
    },
    edge: {
      width: 2,
      color: "rgb(204, 204, 204)",
      arrow: { minSize: 1, maxSize: 1, aspect: 1, hideSize: 1 },
      type: "line"
    }
  }
};

function init(el_id) {
  var el = document.getElementById(el_id);
  //el.addEventListener('mousedown', function(e) { });

  $(el_id).mousedown( function(e){

  });

  $(el_id).mouseup( function(e){

  });

  $(el_id).mousemove( function(e){

  });

  $(el_id).mousemove( function(e){

  });


}

function dimacs2json(dimacs) {
	var json = {};

  var vertex = {};
  var edges = [];
  var max_vertex = 0;
  var edge_v = [];

  var lines = dimacs.split("\n");
  for (var i=0; i<lines.length; i++) {

    var parts = lines[i].split(/ +/);
    if (lines[i].length==0) { continue; }
    if (lines[i][0]=='\n') { continue; }
    if (lines[i][0]=='\r') { continue; }
    if (parts[0] == "c") { continue; }
    if (parts[0] == "p") { continue; }
    if (parts[0] == "e") {
      var n0 = parts[1];
      var n1 = parts[2];
      vertex[n0] = 1;
      vertex[n1] = 1;
      //edges.push( { "data" : { "source":n0, "target":n1 }  });

      edge_v.push([parseInt(n0)-1,parseInt(n1)-1]);

      if (parseInt(n0) > max_vertex) { max_vertex = parseInt(n0); }
      if (parseInt(n1) > max_vertex) { max_vertex = parseInt(n1); }
    }
  }

  var nodes = [];
  for (var i=0; i<max_vertex; i++) {
    var name = String(i+1);
    //nodes.push( { "data" : { "id": name, "name": name } } );
    nodes.push({ "label" : name });
  }

  for (var i=0; i<edge_v.length; i++) {
    edges.push({ source: nodes[edge_v[i][0]], target: nodes[edge_v[i][1]] });
  }

  return { nodes:nodes, edges:edges};
}


var graph = new ccNetViz(document.getElementById('container'), config);

//var V_E = dimacs2json(str);
//var V_E = dimacs2json(str2);
var V_E = dimacs2json(str3);
var nodes = V_E.nodes;
var edges = V_E.edges;

for (var i=0; i<edges.length; i++) {
  if (Math.random()<0.5) {
    edges[i].style = "edgeHighlight";
  }
}

graph.set(nodes, edges, "force");

graph.draw();

 
