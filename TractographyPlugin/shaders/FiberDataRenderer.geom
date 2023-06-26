#version 430
layout (lines) in;

const float PI = 3.1415926535897932384626433832795;
#ifdef DO_STRIPES
layout (triangle_strip, max_vertices = 4) out;
#endif

#ifdef DO_TUBES
#define NUM_SIDES 6
#define MAX_VERTICES 14 // 2*(NUM_SIDES + 1)
layout (triangle_strip, max_vertices = MAX_VERTICES) out;
#endif


in vec4 vert_Tangent[];
in vec4 vert_Position[];

out vec3 geom_Normal;
out vec3 geom_Tangent;
out vec4 geom_Position;
out float geom_SineFlag;
out flat int geom_FiberID;

uniform mat4 u_viewMatrix = mat4(1.0);
uniform mat4 u_projectionMatrix = mat4(1.0);
uniform mat4 u_modelMatrix = mat4(1.0);
uniform mat3 u_normalMatrix = mat3(1.0);
uniform vec3 u_cameraPosition;
uniform float u_fiberWidth;
uniform float u_lengthThreshold;

struct FiberMetaData {
	vec3 avgDirection;
	float length;
	vec3 centerDirection;
	float padding;
};
layout(std430) buffer u_metaData {
	FiberMetaData[] metaData;
};


void main() {
	int fiberID = int(vert_Tangent[0].w);
	if (metaData[fiberID].length < u_lengthThreshold)
		return;

	vec4 v0 = vec4(vert_Position[0].xyz, 1.0);
	vec4 v1 = vec4(vert_Position[1].xyz, 1.0);

#ifdef DO_STRIPES
	vec4 displacement[2];
	vec3 normal[2];

	for (int i = 0; i < 2; ++i) {
		// compute normal
		normal[i] = normalize(vert_Position[i].xyz - u_cameraPosition);

		// gather displacement vector from tangent vector
		displacement[i] = normalize(vec4(cross(normal[i], u_normalMatrix * vert_Tangent[i].xyz), 0.0));
		displacement[i] *= u_fiberWidth;
	}

	geom_FiberID = fiberID;

	geom_Position = v0 + displacement[0];
	gl_Position = u_projectionMatrix * (u_viewMatrix * geom_Position);
	geom_Position.w = vert_Position[0].w;
	geom_Tangent = vert_Tangent[0].xyz;
	geom_Normal = normal[0];
	geom_SineFlag = 0.25;
	EmitVertex();

	geom_Position = v0 - displacement[0];
	gl_Position = u_projectionMatrix * (u_viewMatrix * geom_Position);
	geom_Position.w = vert_Position[0].w;
	geom_Tangent = vert_Tangent[0].xyz;
	geom_Normal = normal[0];
	geom_SineFlag = 0.75;
	EmitVertex();

	geom_Position = v1 + displacement[1];
	gl_Position = u_projectionMatrix * (u_viewMatrix * geom_Position);
	geom_Position.w = vert_Position[1].w;
	geom_Tangent = vert_Tangent[1].xyz;
	geom_Normal = normal[1];
	geom_SineFlag = 0.25;
	EmitVertex();

	geom_Position = v1 - displacement[1];
	gl_Position = u_projectionMatrix * (u_viewMatrix * geom_Position);
	geom_Position.w = vert_Position[1].w;
	geom_Tangent = vert_Tangent[1].xyz;
	geom_Normal = normal[1];
	geom_SineFlag = 0.75;
	EmitVertex();

	EndPrimitive();
#endif

#ifdef DO_TUBES
	mat3 rotMatrix[2];

	for (int i = 0; i < 2; ++i) {
		// calculate correct rotation matrix for pseudo-cylinder footprint:
		vec3 rotBotZ = vert_Tangent[i].xyz; // already normalized!
		bool degenerated = (rotBotZ.x == 0 && rotBotZ.y == 0);
		vec3 rotBotX = degenerated ? vec3(1, 0, 0) : cross(rotBotZ, vec3(0, 0, 1));
		vec3 rotBotY = degenerated ? vec3(0, 1, 0) : cross(rotBotZ, rotBotX);
		rotMatrix[i] = mat3(normalize(rotBotX), normalize(rotBotY), normalize(rotBotZ));
	}

	vec3 normals[NUM_SIDES];
	for (int i = 0; i < NUM_SIDES; ++i) {
		float angle = float(i)/float(NUM_SIDES) * 2.0 * PI;
		normals[i] = vec3(cos(angle) - sin(angle), sin(angle) + cos(angle), 0.0);
	}
	
	geom_FiberID = fiberID;

	for (int i = 0; i < NUM_SIDES; ++i) {
		geom_Normal = normalize(u_normalMatrix * rotMatrix[0] * normals[i]);
		geom_Position = v0 + (vec4(geom_Normal, 0.0) * u_fiberWidth);
		geom_Tangent = vert_Tangent[0].xyz;
		gl_Position = u_projectionMatrix * u_viewMatrix * geom_Position;
		geom_Position.w = vert_Position[0].w;
		EmitVertex();

		geom_Normal = normalize(u_normalMatrix * rotMatrix[1] * normals[i]);
		geom_Position = v1 + (vec4(geom_Normal, 0.0) * u_fiberWidth);
		geom_Tangent = vert_Tangent[1].xyz;
		gl_Position = u_projectionMatrix * u_viewMatrix * geom_Position;
		geom_Position.w = vert_Position[1].w;
		EmitVertex();
	}
	
	geom_Normal = normalize(u_normalMatrix * rotMatrix[0] * normals[0]);
	geom_Position = v0 + (vec4(geom_Normal, 0.0) * u_fiberWidth);
	geom_Tangent = vert_Tangent[0].xyz;
	gl_Position = u_projectionMatrix * u_viewMatrix * geom_Position;
	geom_Position.w = vert_Position[0].w;
	EmitVertex();

	geom_Normal = normalize(u_normalMatrix * rotMatrix[1] * normals[0]);
	geom_Position = v1 + (vec4(geom_Normal, 0.0) * u_fiberWidth);
	geom_Tangent = vert_Tangent[1].xyz;
	gl_Position = u_projectionMatrix * u_viewMatrix * geom_Position;
	geom_Position.w = vert_Position[1].w;
	EmitVertex();

	EndPrimitive;
#endif
}