#include "Mesh.h"


// For the DirectX Math library
using namespace DirectX;


Mesh::Mesh()
{

}

Mesh::Mesh(Vertex* vertexArray, int vertexCount, UINT* indexArray, int indexCount, ID3D11Device* device)
{
	SetBuffers(vertexArray,vertexCount, indexArray, indexCount, device);
	
}

Mesh::Mesh(char * fileName, ID3D11Device * device)
{
	name = strchr(fileName, '.');
	if (strcmp(name, ".obj") == 0)
	{
		std::ifstream obj(fileName);

		// Check for successful open
		if (!obj.is_open())
			return;

		// Variables used while reading the file
		std::vector<XMFLOAT3> positions;     // Positions from the file
		std::vector<XMFLOAT3> normals;       // Normals from the file
		std::vector<XMFLOAT2> uvs;           // UVs from the file
		std::vector<Vertex> verts;           // Verts we're assembling
		std::vector<UINT> indices;           // Indices of these verts
		unsigned int vertCounter = 0;        // Count of vertices/indices
		char chars[100];                     // String for line reading

											 // Still have data left?
		while (obj.good())
		{
			// Get the line (100 characters should be more than enough)
			obj.getline(chars, 100);

			// Check the type of line
			if (chars[0] == 'v' && chars[1] == 'n')
			{
				// Read the 3 numbers directly into an XMFLOAT3
				XMFLOAT3 norm;
				sscanf_s(
					chars,
					"vn %f %f %f",
					&norm.x, &norm.y, &norm.z);

				// Add to the list of normals
				normals.push_back(norm);
			}
			else if (chars[0] == 'v' && chars[1] == 't')
			{
				// Read the 2 numbers directly into an XMFLOAT2
				XMFLOAT2 uv;
				sscanf_s(
					chars,
					"vt %f %f",
					&uv.x, &uv.y);

				// Add to the list of uv's
				uvs.push_back(uv);
			}
			else if (chars[0] == 'v')
			{
				// Read the 3 numbers directly into an XMFLOAT3
				XMFLOAT3 pos;
				sscanf_s(
					chars,
					"v %f %f %f",
					&pos.x, &pos.y, &pos.z);

				// Add to the positions
				positions.push_back(pos);
			}
			else if (chars[0] == 'f')
			{
				// Read the face indices into an array
				unsigned int i[12];
				int facesRead = sscanf_s(
					chars,
					"f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d",
					&i[0], &i[1], &i[2],
					&i[3], &i[4], &i[5],
					&i[6], &i[7], &i[8],
					&i[9], &i[10], &i[11]);

				// - Create the verts by looking up
				//    corresponding data from vectors
				// - OBJ File indices are 1-based, so
				//    they need to be adusted
				Vertex v1;
				v1.Position = positions[i[0] - 1];
				v1.UV = uvs[i[1] - 1];
				v1.Normal = normals[i[2] - 1];

				Vertex v2;
				v2.Position = positions[i[3] - 1];
				v2.UV = uvs[i[4] - 1];
				v2.Normal = normals[i[5] - 1];

				Vertex v3;
				v3.Position = positions[i[6] - 1];
				v3.UV = uvs[i[7] - 1];
				v3.Normal = normals[i[8] - 1];

				// The model is most likely in a right-handed space,
				// especially if it came from Maya.  We want to convert
				// to a left-handed space for DirectX.  This means we 
				// need to:
				//  - Invert the Z position
				//  - Invert the normal's Z
				//  - Flip the winding order
				// We also need to flip the UV coordinate since DirectX
				// defines (0,0) as the top left of the texture, and many
				// 3D modeling packages use the bottom left as (0,0)

				// Flip the UV's since they're probably "upside down"
				v1.UV.y = 1.0f - v1.UV.y;
				v2.UV.y = 1.0f - v2.UV.y;
				v3.UV.y = 1.0f - v3.UV.y;

				// Flip Z (LH vs. RH)
				v1.Position.z *= -1.0f;
				v2.Position.z *= -1.0f;
				v3.Position.z *= -1.0f;

				// Flip normal Z
				v1.Normal.z *= -1.0f;
				v2.Normal.z *= -1.0f;
				v3.Normal.z *= -1.0f;

				// Add the verts to the vector (flipping the winding order)
				verts.push_back(v1);
				verts.push_back(v3);
				verts.push_back(v2);

				// Add three more indices
				indices.push_back(vertCounter); vertCounter += 1;
				indices.push_back(vertCounter); vertCounter += 1;
				indices.push_back(vertCounter); vertCounter += 1;

				// Was there a 4th face?
				if (facesRead == 12)
				{
					// Make the last vertex
					Vertex v4;
					v4.Position = positions[i[9] - 1];
					v4.UV = uvs[i[10] - 1];
					v4.Normal = normals[i[11] - 1];

					// Flip the UV, Z pos and normal
					v4.UV.y = 1.0f - v4.UV.y;
					v4.Position.z *= -1.0f;
					v4.Normal.z *= -1.0f;

					// Add a whole triangle (flipping the winding order)
					verts.push_back(v1);
					verts.push_back(v4);
					verts.push_back(v3);

					// Add three more indices
					indices.push_back(vertCounter); vertCounter += 1;
					indices.push_back(vertCounter); vertCounter += 1;
					indices.push_back(vertCounter); vertCounter += 1;
				}
			}
		}
		SetBuffers(&verts[0], vertCounter, &indices[0], vertCounter, device);

		// Close the file and create the actual buffers
		obj.close();

	}

	else if (strcmp(name, ".bmp") == 0)
	{
		int error, imageSize, i, j, k, index;
		FILE* filePtr;
		unsigned long long count;
		BITMAPFILEHEADER bitmapFileHeader;
		BITMAPINFOHEADER bitmapInfoHeader;
		unsigned char* bitmapImage;
		unsigned char height;


		// Start by creating the array structure to hold the height map data.
		m_heightMap = new HeightMapType[m_terrainWidth * m_terrainHeight];


		// Open the bitmap map file in binary.
		error = fopen_s(&filePtr, fileName, "rb");


		// Read in the bitmap file header.
		count = fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);


		// Read in the bitmap info header.
		count = fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);

		// Calculate the size of the bitmap image data.  
		// Since we use non-divide by 2 dimensions (eg. 257x257) we need to add an extra byte to each line.
		imageSize = m_terrainHeight * ((m_terrainWidth * 3) + 1);

		// Allocate memory for the bitmap image data.
		bitmapImage = new unsigned char[imageSize];


		// Move to the beginning of the bitmap data.
		fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

		// Read in the bitmap image data.
		count = fread(bitmapImage, 1, imageSize, filePtr);


		// Close the file.
		error = fclose(filePtr);


		// Initialize the position in the image data buffer.
		k = 0;

		// Read the image data into the height map array.
		for (j = 0; j<m_terrainHeight; j++)
		{
			for (i = 0; i<m_terrainWidth; i++)
			{
				// Bitmaps are upside down so load bottom to top into the height map array.
				index = (m_terrainWidth * (m_terrainHeight - 1 - j)) + i;

				// Get the grey scale pixel value from the bitmap image data at this location.
				height = bitmapImage[k];

				// Store the pixel value as the height at this point in the height map array.
				m_heightMap[index].y = (float)height;

				// Increment the bitmap image data index.
				k += 3;
			}

			// Compensate for the extra byte at end of each line in non-divide by 2 bitmaps (eg. 257x257).
			k++;
		}

		// Release the bitmap image data now that the height map array has been loaded.
		delete[] bitmapImage;
		bitmapImage = 0;



		index = 0;


		// Loop through all the elements in the height map array and adjust their coordinates correctly.
		for (j = 0; j<m_terrainHeight; j++)
		{
			for (i = 0; i<m_terrainWidth; i++)
			{
				index = (m_terrainWidth * j) + i;

				// Set the X and Z coordinates.
				m_heightMap[index].x = (float)i;
				m_heightMap[index].z = -(float)j;

				// Move the terrain depth into the positive range.  For example from (0, -256) to (256, 0).
				m_heightMap[index].z += (float)(m_terrainHeight - 1);

				// Scale the height.
				m_heightMap[index].y /= m_heightScale;
			}
		}
		Vertex* vertices;
		UINT*   indices;
		int index1, index2, index3, index4;
		int m_vertexCount;

		// Calculate the number of vertices in the 3D terrain model.
		m_vertexCount = (m_terrainHeight - 1) * (m_terrainWidth - 1) * 6;

		// Create the 3D terrain model array.
		m_terrainModel = new ModelType[m_vertexCount];

		vertices = new Vertex[m_vertexCount];
		indices = new unsigned[m_vertexCount];
		// Initialize the index into the height map array.
		index = 0;

		// Load the 3D terrain model with the height map terrain data.
		// We will be creating 2 triangles for each of the four points in a quad.
		for (j = 0; j<(m_terrainHeight - 1); j++)
		{
			for (i = 0; i<(m_terrainWidth - 1); i++)
			{
				// Get the indexes to the four points of the quad.
				index1 = (m_terrainWidth * j) + i;          // Upper left.
				index2 = (m_terrainWidth * j) + (i + 1);      // Upper right.
				index3 = (m_terrainWidth * (j + 1)) + i;      // Bottom left.
				index4 = (m_terrainWidth * (j + 1)) + (i + 1);  // Bottom right.


																// Now create two triangles for that quad.
																// Triangle 1 - Upper left.
				m_terrainModel[index].x = m_heightMap[index1].x;
				m_terrainModel[index].y = m_heightMap[index1].y;
				m_terrainModel[index].z = m_heightMap[index1].z;
				m_terrainModel[index].tu = 0.0f;
				m_terrainModel[index].tv = 0.0f;
				m_terrainModel[index].nx = m_heightMap[index1].nx;
				m_terrainModel[index].ny = m_heightMap[index1].ny;
				m_terrainModel[index].nz = m_heightMap[index1].nz;
				index++;

				// Triangle 1 - Upper right.
				m_terrainModel[index].x = m_heightMap[index2].x;
				m_terrainModel[index].y = m_heightMap[index2].y;
				m_terrainModel[index].z = m_heightMap[index2].z;
				m_terrainModel[index].tu = 1.0f;
				m_terrainModel[index].tv = 0.0f;
				m_terrainModel[index].nx = m_heightMap[index2].nx;
				m_terrainModel[index].ny = m_heightMap[index2].ny;
				m_terrainModel[index].nz = m_heightMap[index2].nz;
				index++;

				// Triangle 1 - Bottom left.
				m_terrainModel[index].x = m_heightMap[index3].x;
				m_terrainModel[index].y = m_heightMap[index3].y;
				m_terrainModel[index].z = m_heightMap[index3].z;
				m_terrainModel[index].tu = 0.0f;
				m_terrainModel[index].tv = 1.0f;
				m_terrainModel[index].nx = m_heightMap[index3].nx;
				m_terrainModel[index].ny = m_heightMap[index3].ny;
				m_terrainModel[index].nz = m_heightMap[index3].nz;
				index++;

				// Triangle 2 - Bottom left.
				m_terrainModel[index].x = m_heightMap[index3].x;
				m_terrainModel[index].y = m_heightMap[index3].y;
				m_terrainModel[index].z = m_heightMap[index3].z;
				m_terrainModel[index].tu = 0.0f;
				m_terrainModel[index].tv = 1.0f;
				m_terrainModel[index].nx = m_heightMap[index3].nx;
				m_terrainModel[index].ny = m_heightMap[index3].ny;
				m_terrainModel[index].nz = m_heightMap[index3].nz;
				index++;

				// Triangle 2 - Upper right.
				m_terrainModel[index].x = m_heightMap[index2].x;
				m_terrainModel[index].y = m_heightMap[index2].y;
				m_terrainModel[index].z = m_heightMap[index2].z;
				m_terrainModel[index].tu = 1.0f;
				m_terrainModel[index].tv = 0.0f;
				m_terrainModel[index].nx = m_heightMap[index2].nx;
				m_terrainModel[index].ny = m_heightMap[index2].ny;
				m_terrainModel[index].nz = m_heightMap[index2].nz;
				index++;

				// Triangle 2 - Bottom right.
				m_terrainModel[index].x = m_heightMap[index4].x;
				m_terrainModel[index].y = m_heightMap[index4].y;
				m_terrainModel[index].z = m_heightMap[index4].z;
				m_terrainModel[index].tu = 1.0f;
				m_terrainModel[index].tv = 1.0f;
				m_terrainModel[index].nx = m_heightMap[index4].nx;
				m_terrainModel[index].ny = m_heightMap[index4].ny;
				m_terrainModel[index].nz = m_heightMap[index4].nz;
				index++;
			}
		}
		for (i = 0; i<m_vertexCount; i++)
		{
			vertices[i].Position = XMFLOAT3(m_terrainModel[i].x, m_terrainModel[i].y, m_terrainModel[i].z);
			vertices[i].UV = XMFLOAT2(m_terrainModel[i].tu, m_terrainModel[i].tv);
			vertices[i].Normal = XMFLOAT3(m_terrainModel[i].nx, m_terrainModel[i].ny, m_terrainModel[i].nz);
			indices[i] = i;
		}
		SetBuffers(&vertices[0], m_vertexCount, &indices[0], m_vertexCount, device);
		delete[] bitmapImage;
		delete[] vertices;
		delete[] indices;
	}



}

Mesh::~Mesh()
{

	if (vertexBuffer) { vertexBuffer->Release(); }
	if (indexBuffer) { indexBuffer->Release(); }
	delete[] m_heightMap;
	delete[] m_terrainModel;

}

void Mesh::SetBuffers(Vertex * vertexArray, int vertexCount, UINT * indexArray, int indexCount, ID3D11Device * device)
{
	CalculateTangents(vertexArray, vertexCount, indexArray, indexCount);
	
	m_indexCount = indexCount;

	// Create the VERTEX BUFFER description -----------------------------------
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex) * vertexCount;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	// Create the proper struct to hold the initial vertex data
	D3D11_SUBRESOURCE_DATA initialVertexData;
	initialVertexData.pSysMem = vertexArray;

	// Actually create the buffer with the initial data
	device->CreateBuffer(&vbd, &initialVertexData, &vertexBuffer);



	// Create the INDEX BUFFER description ------------------------------------
	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(int) * indexCount;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;

	// Create the proper struct to hold the initial index data
	D3D11_SUBRESOURCE_DATA initialIndexData;
	initialIndexData.pSysMem = indexArray;

	// Actually create the buffer with the initial data
	device->CreateBuffer(&ibd, &initialIndexData, &indexBuffer);
}

ID3D11Buffer* Mesh::GetVertexBuffer()
{
	return vertexBuffer;
}

ID3D11Buffer* Mesh::GetIndexBuffer()
{
	return indexBuffer;
}

int Mesh::GetIndexCount()
{
	return m_indexCount;
}

void Mesh::CalculateTangents(Vertex* verts, int numVerts, unsigned int* indices, int numIndices)
{
	// Reset tangents
	for (int i = 0; i < numVerts; i++)
	{
		verts[i].Tangent = XMFLOAT3(0, 0, 0);
	}

	// Calculate tangents one whole triangle at a time
	for (int i = 0; i < numVerts;)
	{
		// Grab indices and vertices of first triangle
		unsigned int i1 = indices[i++];
		unsigned int i2 = indices[i++];
		unsigned int i3 = indices[i++];
		Vertex* v1 = &verts[i1];
		Vertex* v2 = &verts[i2];
		Vertex* v3 = &verts[i3];

		// Calculate vectors relative to triangle positions
		float x1 = v2->Position.x - v1->Position.x;
		float y1 = v2->Position.y - v1->Position.y;
		float z1 = v2->Position.z - v1->Position.z;

		float x2 = v3->Position.x - v1->Position.x;
		float y2 = v3->Position.y - v1->Position.y;
		float z2 = v3->Position.z - v1->Position.z;

		// Do the same for vectors relative to triangle uv's
		float s1 = v2->UV.x - v1->UV.x;
		float t1 = v2->UV.y - v1->UV.y;

		float s2 = v3->UV.x - v1->UV.x;
		float t2 = v3->UV.y - v1->UV.y;

		// Create vectors for tangent calculation
		float r = 1.0f / (s1 * t2 - s2 * t1);

		float tx = (t2 * x1 - t1 * x2) * r;
		float ty = (t2 * y1 - t1 * y2) * r;
		float tz = (t2 * z1 - t1 * z2) * r;

		// Adjust tangents of each vert of the triangle
		v1->Tangent.x += tx;
		v1->Tangent.y += ty;
		v1->Tangent.z += tz;

		v2->Tangent.x += tx;
		v2->Tangent.y += ty;
		v2->Tangent.z += tz;

		v3->Tangent.x += tx;
		v3->Tangent.y += ty;
		v3->Tangent.z += tz;
	}

	// Ensure all of the tangents are orthogonal to the normals
	for (int i = 0; i < numVerts; i++)
	{
		// Grab the two vectors
		XMVECTOR normal = XMLoadFloat3(&verts[i].Normal);
		XMVECTOR tangent = XMLoadFloat3(&verts[i].Tangent);

		// Use Gram-Schmidt orthogonalize
		tangent = XMVector3Normalize(
			tangent - normal * XMVector3Dot(normal, tangent));

		// Store the tangent
		XMStoreFloat3(&verts[i].Tangent, tangent);
	}
}