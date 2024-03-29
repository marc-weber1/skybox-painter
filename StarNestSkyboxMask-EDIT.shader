// Upgrade NOTE: replaced '_Object2World' with 'unity_ObjectToWorld'
// Upgrade NOTE: replaced 'mul(UNITY_MATRIX_MVP,*)' with 'UnityObjectToClipPos(*)'

//Star Nest algorithm by Pablo Román Andrioli
//Unity 5.x shader by Jonathan Cohen
//This content is under the MIT License.
//
//Original Shader:
//https://www.shadertoy.com/view/XlfGRj
//
//This shader uses the same algorithm in 3d space to render a skybox.

Shader "Skybox/StarNestMask" {
	Properties {
		//Gradient mask
		_Mask ("Clamp Mask", Cube) = "white" {}
		
		_Color ("Main Color", Color) = (1,1,1,1)	
		
		//Scrolls in this direction over time. Set 'w' to zero to stop scrolling.
		_Scroll ("Scrolling direction (x,y,z) * w * time", Vector) = (1.3, 1, .6, .01)
		
		//Center position in space and time.
		_Center ("Center Position (x, y, z, time)", Vector) = (1, .3, .5, 0)
		
		//How much does camera position cause the effect to scroll?
		_CamScroll ("Camera Scroll", Float) = 0
		
		//Does rotation apply?
		_Rotation ("Rotation axis (x,y,z) * w * time", Vector) = (0, 0, 0, .01)
		
		//Iterations of inner loop. 
		//The higher this is, the more distant objects get rendered.
		_Iterations ("Iterations", Range(1, 30)) = 15
		
		//Volumetric rendering steps. Each 'step' renders more objects at all distances.
		//This has a higher performance hit than iterations.
		_Volsteps ("Volumetric Steps", Range(1,20)) = 8
		
		//Magic number. Best values are around 400-600.
		_Formuparam ("Formuparam", Float) = 420
		
		//How much farther each volumestep goes
		_StepSize ("Step Size", Float) = 355
		
		//Fractal repeating rate
		//Low numbers are busy and give lots of repititio
		//High numbers are very sparce
		_Tile ("Tile", Float) = 700
		
		//Brightness scale.
		_Brightness ("Brightness", Float) = .5
		//Abundance of Dark matter (in the distance). 
		//Visible with Volsteps >= 8 (at 7 its really, really hard to see)
		_Darkmatter ("Dark Matter", Float) = 555
		//Brightness of distant objects (or dim) are distant objects
		//Ironically, Also affets brightness of 'darkmatter'
		_Distfading ("Distance Fading", Float) = 55
		//How much color is present?
		_Saturation ("Saturation", Float) = 77
		
	}

	SubShader {
		Tags { "Queue"="Background" "RenderType"="Background" "PreviewType"="Skybox" }
		Cull Off ZWrite Off
		
		Pass {
			CGPROGRAM
			#pragma vertex vert
			#pragma fragment frag
			#include "UnityCG.cginc"
			
			static const float kInnerRadius = 1.0;
			static const float kCameraHeight = 0.0001;

			uniform samplerCUBE _Mask;
			uniform half4 _Mask_HDR;
			
			uniform fixed4 _Color;
			
			uniform int _Volsteps;
			uniform int _Iterations;
			
			uniform float4 _Scroll;
			uniform float4 _Center;
			uniform float _CamScroll;
			uniform float4 _Rotation;
			
			uniform float _Formuparam;
			uniform float _StepSize;

			uniform float _Tile;

			uniform float _Brightness;
			uniform float _Darkmatter;
			uniform float _Distfading;
			uniform float _Saturation;
			const float PI = 3.14159265359;
			const float pi2 = 1.57079632679;
			const float pi4 = 0.78539816339;
			
			float map(float val, float2 a, float2 b) {
				if (val < a.x) { return b.x; }
				if (val > a.y) { return b.y; }
				
				float ad = abs(a.y - a.x);
				float bd = abs(b.y - b.x);
				
				float p = (val - a.x) / ad;
				return b.x + p * bd;
			}
			
			float3 latLongAlt(float3 pos) {
				float flatLength = sqrt(pos.x * pos.x + pos.z * pos.z);
				
				float3 lla = float3(0., 0., 0.);
				lla.x = atan2(pos.y, flatLength);
				lla.y = atan2(pos.z, pos.x);
				lla.z = length(pos);
				
				return lla;
			}
			
			struct appdata_t {
				float4 vertex : POSITION;
				float2 uv : TEXCOORD0;
			};
			struct v2f {
				float4 pos : SV_POSITION;
				half3 rayDir : TEXCOORD0;	// Vector for incoming ray, normalized ( == -eyeRay )
				float3 uv : TEXCOORD1;
			}; 
			
			v2f vert(appdata_t v) {
				v2f OUT;
				OUT.pos = UnityObjectToClipPos(v.vertex);
				float3 cameraPos = float3(0,kInnerRadius + kCameraHeight,0); 	// The camera's current position
			
				// Get the ray from the camera to the vertex and its length (which is the far point of the ray passing through the atmosphere)
				float3 eyeRay = normalize(mul((float3x3)unity_ObjectToWorld, v.vertex.xyz));

				OUT.rayDir = half3(eyeRay);
				
				
				// debugginmng
				OUT.uv = v.vertex.xyz;
				
				
				return OUT;
			}
			
			half4 frag (v2f IN) : SV_Target {
				half3 col = half3(0, 0, 0);
				float4 pos = IN.pos;
				half3 dir = IN.rayDir;
				
				float time = _Center.w + _Time.x;
				
				//Un-scale parameters (source parameters for these are mostly in 0...1 range)
				//Scaling them up makes it much easier to fine-tune shader in the inspector.
				float brightness = _Brightness / 1000;
				float stepSize = _StepSize / 1000;
				float3 tile = abs(float3(_Tile, _Tile, _Tile)) / 1000;
				float formparam = _Formuparam / 1000;
				
				float darkmatter = _Darkmatter / 100;
				float distFade = _Distfading / 100;
				
				float3 from = _Center.xyz;
				
				//scroll over time
				from += _Scroll.xyz * _Scroll.w * time;
				//scroll from camera position
				from += _WorldSpaceCameraPos * _CamScroll * .00001;
				
				
				//Apply rotation if enabled
				float3 rot = _Rotation.xyz * _Rotation.w * time * .1;
				if (length(rot) > 0) {
					float2x2 rx = float2x2(cos(rot.x), sin(rot.x), -sin(rot.x), cos(rot.x));
					float2x2 ry = float2x2(cos(rot.y), sin(rot.y), -sin(rot.y), cos(rot.y));
					float2x2 rz = float2x2(cos(rot.z), sin(rot.z), -sin(rot.z), cos(rot.z));

					dir.xy = mul(rz, dir.xy);
					dir.xz = mul(ry, dir.xz);
					dir.yz = mul(rx, dir.yz);
					from.xy = mul(rz, from.xy);
					from.xz = mul(ry, from.xz);
					from.yz = mul(rx, from.yz);
				}
				
				
				//volumetric rendering
				float s = 0.1, fade = 1.0;
				float3 v = float3(0, 0, 0);
				for (int r = 0; r < _Volsteps; r++) {
					float3 p = abs(from + s * dir * .5);
					
					p = abs(float3(tile - fmod(p, tile*2)));
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
					v += float3(s, s*s, s*s*s*s) * a * brightness * fade;
					
					// distance fading
					fade *= distFade;
					s += stepSize;
				}
				
				float len = length(v);
				//Quick saturate
				v = lerp(float3(len, len, len), v, _Saturation / 100);
				v *= .01;
				v.xyz *= _Color.xyz;
				
				//Reduce overbright, keep a little 
				v = clamp(v, 0, 1.15);
				
				
				float3 lla = latLongAlt(IN.rayDir);
				float pi = 3.14159265359;
				float2 uuv = float2(0., 0.);
				uuv.x = map(lla.y, float2(-pi, pi), float2(0, 1));
				uuv.y = map(lla.x, float2(-pi, pi)/2., float2(0, 1));
				
				// float blend = tex2D(_Mask, uuv);
				float blend = DecodeHDR( texCUBE(_Mask, IN.uv), _Mask_HDR);
				v.rgb *= blend;
				
				return half4(v, 1.0);
			}
			
			
			
			ENDCG
		}
		
		
	}

	Fallback Off
}