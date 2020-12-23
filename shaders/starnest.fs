#version 330 core

in vec2 TexCoords;
in vec3 Normal;

out vec4 FragColor;


const float kInnerRadius = 1.0;
const float kCameraHeight = 0.0001;

uniform sampler2D _Mask;
uniform float _Time;

const vec4 _Color = vec4(1,1,1,1);

const int _Volsteps = 9;
const int _Iterations = 14;

const vec4 _Scroll = vec4(1.3,1,0.6,0.01);
const vec4 _Center = vec4(1,0.3,0.5,0);
const float _CamScroll = 0;
const vec4 _Rotation = vec4(0,0,0,0.01);

const float _Formuparam = 420;
const float _StepSize = 355;

const float _Tile = 700;

const float _Brightness = 0.5;
const float _Darkmatter = 555;
const float _Distfading = 55;
const float _Saturation = 77;


const float pi = 3.14159265359;
const float pi2 = 1.57079632679;
const float pi4 = 0.78539816339;


float map(float val, vec2 a, vec2 b) {
	if (val < a.x) { return b.x; }
	if (val > a.y) { return b.y; }
	
	float ad = abs(a.y - a.x);
	float bd = abs(b.y - b.x);
	
	float p = (val - a.x) / ad;
	return b.x + p * bd;
}

vec3 latLongAlt(vec3 pos) {
	float flatLength = sqrt(pos.x * pos.x + pos.z * pos.z);
	
	vec3 lla = vec3(0., 0., 0.);
	lla.x = atan(pos.y, flatLength);
	lla.y = atan(pos.z, pos.x);
	lla.z = length(pos);
	
	return lla;
}

/*struct appdata_t {
	float4 vertex : POSITION;
};
struct v2f {
	float4 pos : SV_POSITION;
	half3 rayDir : TEXCOORD0;	// Vector for incoming ray, normalized ( == -eyeRay )
}; 

v2f vert(appdata_t v) {
	v2f OUT;
	OUT.pos = UnityObjectToClipPos(v.vertex);
	float3 cameraPos = float3(0,kInnerRadius + kCameraHeight,0); 	// The camera's current position

	// Get the ray from the camera to the vertex and its length (which is the far point of the ray passing through the atmosphere)
	float3 eyeRay = normalize(mul((float3x3)unity_ObjectToWorld, v.vertex.xyz));

	OUT.rayDir = half3(eyeRay);
	
	
	return OUT;
}*/

void main() {
	vec3 dir = -Normal;
	
	float time = _Center.w + _Time.x;
	
	//Un-scale parameters (source parameters for these are mostly in 0...1 range)
	//Scaling them up makes it much easier to fine-tune shader in the inspector.
	float brightness = _Brightness / 1000;
	float stepSize = _StepSize / 1000;
	vec3 tile = abs(vec3(_Tile, _Tile, _Tile)) / 1000;
	float formparam = _Formuparam / 1000;
	
	float darkmatter = _Darkmatter / 100;
	float distFade = _Distfading / 100;
	
	vec3 from = _Center.xyz;
	
	//scroll over time
	from += _Scroll.xyz * _Scroll.w * time;
	//scroll from camera position
	//from += _WorldSpaceCameraPos * _CamScroll * .00001;
	
	
	//Apply rotation if enabled
	vec3 rot = _Rotation.xyz * _Rotation.w * time * .1;
	if (length(rot) > 0) {
		mat2 rx = mat2( cos(rot.x), -sin(rot.x), 
						sin(rot.x), cos(rot.x) );
		mat2 ry = mat2( cos(rot.y), -sin(rot.y),
						sin(rot.y), cos(rot.y) );
		mat2 rz = mat2( cos(rot.z), -sin(rot.z),
						sin(rot.z), cos(rot.z) );

		dir.xy = mul(rz, dir.xy);
		dir.xz = mul(ry, dir.xz);
		dir.yz = mul(rx, dir.yz);
		from.xy = mul(rz, from.xy);
		from.xz = mul(ry, from.xz);
		from.yz = mul(rx, from.yz);
	}
	
	
	//volumetric rendering
	float s = 0.1, fade = 1.0;
	vec3 v = vec3(0, 0, 0);
	for (int r = 0; r < _Volsteps; r++) {
		vec3 p = abs(from + s * dir * .5);
		
		p = abs(vec3(tile - mod(p, tile*2)));
		float pa,a = pa = 0.;
		for (int i = 0; i < _Iterations; i++) {
			p = abs(p) / dot(p, p) - formparam;
			a += abs(length(p) - pa);
			pa = length(p);
		}
		//Dark matter
		float dm = max(0., darkmatter - a * a * .001);
		if (r > 6) { fade *= 1. - dm; } // Render distant darkmatter
		a *= a * a; //add contrast
		
		v += fade;
		
		// coloring based on distance
		v += vec3(s, s*s, s*s*s*s) * a * brightness * fade;
		
		// distance fading
		fade *= distFade;
		s += stepSize;
	}
	
	float len = length(v);
	//Quick saturate
	v = mix(vec3(len, len, len), v, _Saturation / 100);
	v *= .01;
	v.xyz *= _Color.xyz;
	
	//Reduce overbright, keep a little 
	v = clamp(v, 0, 1.15);
	
	
	vec3 lla = latLongAlt(-Normal);
	vec2 uuv = vec2(0., 0.);
	uuv.x = map(lla.y, vec2(-pi, pi), vec2(0, 1));
	uuv.y = map(lla.x, vec2(-pi, pi)/2., vec2(0, 1));
	
	float blend = texture2D(_Mask, TexCoords).r;
	v.rgb *= blend;
	
	FragColor = vec4(v, 1.0);
}