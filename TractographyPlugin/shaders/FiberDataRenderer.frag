#version 430

in vec3 geom_Normal;
in vec3 geom_Tangent;
in vec4 geom_Position;
in float geom_SineFlag;
in flat int geom_FiberID;

out vec4 out_Color;

uniform vec3 u_cameraPosition;
uniform vec3 u_lookVector;
uniform float u_minAnisotropy;
uniform float u_maxAnisotropy;
uniform int u_coloringMode;
uniform mat3 u_normalMatrix = mat3(1.0);
uniform mat4 u_viewMatrix = mat4(1.0);
uniform mat4 u_modelMatrix;

struct FiberMetaData {
	vec3 avgDirection;
	float length;
	vec3 centerDirection;
	float padding;
};
layout(std430) buffer u_metaData {
	FiberMetaData[] metaData;
};

/**
 * Calculate Phong terms for simple OpenGL line primitives according to
 * the work of Zöckler, Stalling and Hege in "Interactive Visualization Of
 * 3D-Vector Fields Using Illuminated Stream Lines", from 1996.
 */
vec3 phongShadingForLines(vec3 tangent, vec3 view, vec3 light, float shininess) {
	// normalize the vectors again which are interpolated per pixel and are
	// therefore different for each fragment when transfered from vertex to
	// fragment shader!
	// light does not need to be re-normalized, as it remains the same for
	// all fragments.
	//
	vec3 t = normalize(tangent);  // normalize again as the normal is interpolated per pixel!
	vec3 v = normalize(view);     // the same for the view vector: it is interpolated per pixel!
	float LdotT = clamp(dot(light, t), -1.0, 1.0);
	float NdotL = max(sqrt(1.0 - (LdotT * LdotT)), 0.0);

	float VdotT = clamp(dot(v, t), -1.0, 1.0);
	float VdotN = max(sqrt(1.0 - (VdotT * VdotT)), 0.0);
	float RdotV = (LdotT * VdotT) - (NdotL * VdotN);
	float specular = max(pow(RdotV, shininess), 0.0);
	//const float p = 2.0;
	const float p = 4.8;
	return vec3(1.0, pow(NdotL, p), specular);
}

vec3 phongShadingForFaces(vec3 N, vec3 V, vec3 L, float shininess) {
	float NdotL = max(dot(N, L), 0.0);
	vec3 R = normalize(reflect(-L, N));
	float NdotH = max(dot(V, R), 0.0);
	return vec3(1.0, NdotL, pow(NdotH, shininess));
}

/**
 * Applies tube-like texture to triangle strip
 */
vec4 applyTubeTexture(vec4 color) {
	const float PI = 3.1415926535897932384626433832795;
	float val = sin(geom_SineFlag * PI);
	return vec4(color.rgb * (val + pow(val, 16.0)), color.a);
}


void saturateColor(inout vec4 color, in float mixval) {
	float luminance = (color.r + color.g + color.b) / 3.0;
	color.rgb = mix(vec3(luminance), color.rgb, sqrt(mixval));
}

// From https://stackoverflow.com/questions/15095909/from-rgb-to-hsv-in-opengl-glsl
vec3 hsv2rgb(vec3 c)
{
	vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
	vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
	return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main() {
	const float PI = 3.1415926535897932384626433832795;

	float mixval = 1.0;
	if (u_maxAnisotropy > u_minAnisotropy)
		mixval = smoothstep(u_minAnisotropy, u_maxAnisotropy, geom_Position.w);

	switch (u_coloringMode) {
		case 0: // local tangent color
			out_Color = vec4(normalize(abs(geom_Tangent)), 1.0);
			saturateColor(out_Color, mixval);
			break;
		case 1: // average tangent color
			out_Color = vec4(normalize(abs(metaData[geom_FiberID].avgDirection.xyz)), 1.0);
			saturateColor(out_Color, mixval);
			break;
		case 2: // center tangent color
			out_Color = vec4(normalize(abs(metaData[geom_FiberID].centerDirection.xyz)), 1.0);
			saturateColor(out_Color, mixval);
			break;
		case 3: // view dependent hsv coloring
			vec2 colorDir = normalize((u_viewMatrix * u_modelMatrix * vec4(geom_Tangent, 0.0)).xy);
			float angle =  colorDir.x != 0.0 ? atan(colorDir.y, colorDir.x) : 0.0;
			if(angle > 0.0)
				angle -= PI;
			angle = 1.0 - angle / PI;
			out_Color = vec4(hsv2rgb(vec3(angle, 1.0, 1.0)), 1.0);
			saturateColor(out_Color, mixval);
			break;
		default:
			out_Color = vec4(1.0);
			saturateColor(out_Color, mixval);
			break;
	}

#ifdef DO_STRIPES
	out_Color = applyTubeTexture(out_Color);
#endif

	vec3 phongTerms = phongShadingForFaces(normalize(-geom_Normal), normalize(u_lookVector), normalize(geom_Position.xyz - u_cameraPosition), 5.0);
	out_Color.rgb *= (0.4 * phongTerms.x) + (0.65 * phongTerms.y) + (0.65 * phongTerms.z);
	gl_FragDepth = gl_FragCoord.z;
}
