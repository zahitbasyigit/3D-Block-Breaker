attribute  vec4 vPosition;
attribute  vec4 vColor;
attribute  vec3 vNormal;

// output values that will be interpretated per-fragment
varying  vec3 fN;
varying  vec3 fV;
varying  vec3 fL;
varying  vec4 color;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform vec4 ucolor;
uniform vec4 LightPosition;


void main()
{
    // Transform vertex position into camera (eye) coordinates
    vec3 pos = (model * vPosition).xyz;
    
    fN = (model * vec4(vNormal, 0.0)).xyz; // normal direction in camera coordinates

    fV = -pos; //viewer direction in camera coordinates

    fL = LightPosition.xyz; // light direction

    if( LightPosition.w != 0.0 ) {
        fL = LightPosition.xyz - pos;  //directional light source
    }

	gl_Position = projection * view * model * vPosition;
	color = ucolor;
} 