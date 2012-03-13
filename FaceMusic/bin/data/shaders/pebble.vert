varying vec2 position, center;

void main() {
	gl_FrontColor = gl_Color;
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	
	position = (gl_ModelViewMatrix * gl_Vertex).xy;
	
	// get the position at 0,0, instead of passing in pebbleCenter
	// equivalent to (gl_ModelViewMatrix  * vec4(0, 0, 0, 1)).xy;
	center = gl_ModelViewMatrix[3].xy;
}