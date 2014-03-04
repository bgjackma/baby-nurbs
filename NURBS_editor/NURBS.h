#pragma once

#include <vector>
#include <gl/freeglut.h>
#include <cassert>
#include <glm/glm.hpp>
#include "MiscTypes.h"
#include <iostream>

template<int K>
float EvalB(float u, int i, const std::vector<float>& uKn)
{
   float ratio1 = (u - uKn[i]) / (uKn[i + K - 1] - uKn[i]);
   float b1 = EvalB<K - 1>(u, i, uKn);
   float ratio2 = (uKn[i + K] - u) / (uKn[i + K] - uKn[i+1]);
   float b2 = EvalB<K - 1>(u, i + 1, uKn);
   float val = ratio1 * b1 + ratio2 * b2;
   return val;
}

template<>
inline float EvalB<1>(float u, int i, const std::vector<float>& uKn)
{
   if (u >= uKn[i] && u < uKn[i+1])
      return 1;
   else
      return 0;
}

template <int D>
class NURBS
{
public:
   NURBS(int uPoints, int vPoints);

   void DrawSelectedPoints();
   void DrawControlMesh();
   void DrawSurface();

   //selects first control point along unit ray
   //cutoff = max distance from ray
   void SelectOnRay(Ray ray, float cutoff);
   //moves currently selected point by displacement
   void MoveSelected(const glm::vec3& displacement);

   //returns position of selected control point
   glm::vec3 GetSelectedPos() const;
   //sets position of selected control point to pos
   void SetSelectedPos(const glm::vec3& pos);
   //returns weight of selected control point
   float GetSelectedWeight() const { return controlWeights[selectedPoint]; }
   //sets weight of selected control point to w
   void SetSelectedWeight(float w) { controlWeights[selectedPoint] = w; }

   void SetSize(int uPoints, int vPoints, float width, float height);

   //resets all control point weights to 1
   void ResetWeights();

   //makes all knots uniform with separations of 1
   void MakeKnotsUniform();
   //rebuils control mesh from control points
   void ReloadControlMesh();
   //defines polygon indices for the surface
   void ReloadSurfaceIndices();
   //recalculates the NURBS surface
   void RecalculateSurface();
   //calculates surface normals
   void FindNormals();

   ~NURBS() {}
private:
   std::vector<GLfloat> control;
   std::vector<float> controlWeights;
   std::vector<GLushort> controlWires;
   int selectedPoint;
   std::vector<float> uKnots;
   std::vector<float> vKnots;
   std::vector<GLfloat> surfaceVerts;
   std::vector<GLushort> surfaceIndices;
   std::vector<GLfloat> surfaceNormals;

   int uPoints, vPoints;
   int uSegs, vSegs;
   int resolution;
};

template <int D>
NURBS<D>::NURBS(int uPoints, int vPoints)
: resolution(10), selectedPoint(0)
{
   SetSize(uPoints, vPoints, 1.0, 1.0);
}


template<int D>
void NURBS<D>::ResetWeights()
{
   controlWeights.assign(controlWeights.size(), 1.0f);
}


template<int D>
void NURBS<D>::SetSize(int uPoints, int vPoints, float width, float height)
{
   this->uPoints = uPoints;
   this->vPoints = vPoints;
   control.resize(3 * uPoints * vPoints);
   controlWeights = std::vector<float>(uPoints * vPoints, 1.0f);
   for (int i = 0; i < uPoints; ++i)
   {
      for (int j = 0; j < vPoints; ++j)
      {
         control[3 * (i * vPoints + j)] = i * width / (uPoints - 1) - width / 2;
         control[3 * (i * vPoints + j) + 1] = j * height / (vPoints - 1) - height / 2;
         control[3 * (i * vPoints + j) + 2] = 0;
      }
   }
   uSegs = (uPoints - D + 1) * resolution + 1;
   vSegs = (vPoints - D + 1) * resolution + 1;
   
   //setup knots
   uKnots.resize(uPoints + D + 1);
   vKnots.resize(vPoints + D + 1);
   MakeKnotsUniform();

   //setup meshes
   ReloadControlMesh();
   ReloadSurfaceIndices();
   surfaceVerts.resize(3 * uSegs * vSegs);
   //calculate exciting flat surface
   RecalculateSurface();
}

template<int D>
void NURBS<D>::ReloadControlMesh()
{
   controlWires.clear();
   for (int i = 0; i < uPoints; ++i)
   {
      for (int j = 0; j < vPoints - 1; ++j)
      {
         controlWires.push_back(i * vPoints + j);
         controlWires.push_back(i * vPoints + j + 1);
      }
   }
   for (int j = 0; j < vPoints; ++j)
   {
      for (int i = 0; i < uPoints - 1; ++i)
      {
         controlWires.push_back(i * vPoints + j);
         controlWires.push_back((i + 1) * vPoints + j);
      }
   }
}


template<int D>
void NURBS<D>::MakeKnotsUniform()
{
   for (int i = 0; i <= uPoints + D; ++i)
   {
      uKnots[i] = (float)i;
   }
   for (int j = 0; j <= vPoints + D; ++j)
   {
      vKnots[j] = (float)j;
   }
   return;
}



template<int D>
void NURBS<D>::ReloadSurfaceIndices()
{
   surfaceIndices.clear();
   for (int col = 0; col < uSegs - 1; ++col)
   {
      for (int row = 0; row < vSegs - 1; ++row)
      {
         surfaceIndices.push_back(col * vSegs + row);
         surfaceIndices.push_back(col * vSegs + row + 1);
         surfaceIndices.push_back((col + 1) * vSegs + row);
         surfaceIndices.push_back((col + 1) * vSegs + row + 1);
         surfaceIndices.push_back((col + 1) * vSegs + row);
         surfaceIndices.push_back(col * vSegs + row + 1);
      }
   }
}

template <int D>
void NURBS<D>::DrawControlMesh()
{
   glDisable(GL_LIGHTING);
   glEnableClientState(GL_VERTEX_ARRAY);
   glVertexPointer(3, GL_FLOAT, 0, control.data());
   glColor3f(1, 1, 0.8f);
   glDrawElements(GL_LINES, controlWires.size(), GL_UNSIGNED_SHORT, controlWires.data());
   glDisableClientState(GL_VERTEX_ARRAY);
}

template <int D>
void NURBS<D>::DrawSelectedPoints()
{
   glDisable(GL_LIGHTING);
   glEnableClientState(GL_VERTEX_ARRAY);
   glVertexPointer(3, GL_FLOAT, 0, control.data());
   glColor3f(1, 0, 0);
   glDrawElements(GL_POINTS, 1, GL_UNSIGNED_SHORT, &selectedPoint);
   glDisableClientState(GL_VERTEX_ARRAY);
}

template <int D>
void NURBS<D>::DrawSurface()
{
   glEnable(GL_LIGHTING);
   glPushMatrix();
   glLoadIdentity();
   glRotatef(0, 1, 1, 1);
   glEnableClientState(GL_VERTEX_ARRAY);
   glEnableClientState(GL_NORMAL_ARRAY);
   glVertexPointer(3, GL_FLOAT, 0, surfaceVerts.data());
   glNormalPointer(GL_FLOAT, 0, surfaceNormals.data());
   glColor3f(1, 1, 1);
   glDrawElements(GL_TRIANGLES, surfaceIndices.size(), GL_UNSIGNED_SHORT, surfaceIndices.data());
   glDisableClientState(GL_VERTEX_ARRAY);
   glDisableClientState(GL_NORMAL_ARRAY);
   glPopMatrix();
}

template <int D>
void NURBS<D>::FindNormals()
{
	//calculate face surfaceNormals
	std::vector<GLfloat> faceNorms;
	for(unsigned int i = 0; i < surfaceIndices.size(); i += 3) {
		GLfloat ux = surfaceVerts[3*surfaceIndices[i+1]] - surfaceVerts[3*surfaceIndices[i]];
		GLfloat uy = surfaceVerts[3*surfaceIndices[i+1]+1] - surfaceVerts[3*surfaceIndices[i]+1];
		GLfloat uz = surfaceVerts[3*surfaceIndices[i+1]+2] - surfaceVerts[3*surfaceIndices[i]+2];
		GLfloat vx = surfaceVerts[3*surfaceIndices[i+2]] - surfaceVerts[3*surfaceIndices[i]];
		GLfloat vy = surfaceVerts[3*surfaceIndices[i+2]+1] - surfaceVerts[3*surfaceIndices[i]+1];
		GLfloat vz = surfaceVerts[3*surfaceIndices[i+2]+2] - surfaceVerts[3*surfaceIndices[i]+2];

		GLfloat nx = uy*vz - uz*vy;
		GLfloat ny = uz*vx - ux*vz;
		GLfloat nz = ux*vy - uy*vx;
		GLfloat correction = 1 / sqrt(nx*nx + ny*ny + nz*nz);

		faceNorms.push_back(nx * correction);
		faceNorms.push_back(ny * correction);
		faceNorms.push_back(nz * correction);
	}
	//convert to vertex surfaceNormals
	surfaceNormals.assign(surfaceVerts.size(), 0.0);
	std::vector<unsigned short> surfaceIndicesOnVertex(surfaceVerts.size() / 3, 0);
	for(unsigned int i = 0; i < surfaceIndices.size(); i += 3)
	{
		for(unsigned int j = 0; j < 3; ++j)
		{
			surfaceNormals[3 * surfaceIndices[i+j]] += faceNorms[i];
			surfaceNormals[3 * surfaceIndices[i+j] + 1] += faceNorms[i+1];
			surfaceNormals[3 * surfaceIndices[i+j] + 2] += faceNorms[i+2];
			++surfaceIndicesOnVertex[surfaceIndices[i+j]];
		}
	}

	//normalize
	for(unsigned int i = 0; i < surfaceIndicesOnVertex.size(); ++i)
	{
		float corr = 1.0f / surfaceIndicesOnVertex[i];
		surfaceNormals[i * 3] *= corr;
		surfaceNormals[i * 3 + 1] *= corr;
		surfaceNormals[i * 3 + 2] *= corr;
	}

	return;
}

template <int D>
void NURBS<D>::RecalculateSurface()
{
   float u = uKnots[D - 1];
   float v = vKnots[D - 1];
   float uStep = (uKnots[uPoints] - u) / (uSegs - 1);
   float vStep = (vKnots[vPoints] - v) / (vSegs - 1);
   //starting value for i and j, per loop
   int iStart = D - 1;
   int jStart = D - 1;
   for (int c1 = 0; c1 < uSegs; ++c1)
   {
      jStart = D - 1;
      v = vKnots[D-1];
      for (int c2 = 0; c2 < vSegs; ++c2)
      {
         int vIndex = 3 * (c1 * vSegs + c2);
         surfaceVerts[vIndex] = 0;
         surfaceVerts[vIndex+1] = 0;
         surfaceVerts[vIndex+2] = 0;
         float bTotal = 0;
         float totalWeight = 0;
         for (int i = 1 - D; i < D; ++i)
         {
            if (iStart + i < 0 || iStart + i >= uPoints)
               continue;
            float Bi = EvalB<D>(u, i + iStart, uKnots);
            for (int j = 1 - D; j < D; ++j)
            {
               if (jStart + j < 0 || jStart + j >= vPoints)
                  continue;
               float Bj =  EvalB<D>(v, j + jStart, vKnots);
               //index of control point
               int pIJ = ((i + iStart) * vPoints + j + jStart);
               float weightedB = Bi * Bj * controlWeights[pIJ];
               //adjust index for array of XYZ vectors
               pIJ *= 3;
               surfaceVerts[vIndex] += control[pIJ] * weightedB;
               surfaceVerts[vIndex + 1] += control[pIJ + 1] * weightedB;
               surfaceVerts[vIndex + 2] += control[pIJ + 2] * weightedB;
               totalWeight += weightedB;
            }
         }
         float inverseWeight = 1 / totalWeight;
         surfaceVerts[vIndex] *= inverseWeight;
         surfaceVerts[vIndex + 1] *= inverseWeight;
         surfaceVerts[vIndex + 2] *= inverseWeight;
         v += vStep;
         if (v > vKnots[jStart+1])
            jStart++;
      }
      u += uStep;
      if (u > uKnots[iStart+1])
         iStart++;
   }

   FindNormals();
}


template<int D>
void NURBS<D>::SelectOnRay(Ray ray, float cutoff)
{
   std::cerr << "Ray Origin: " << ray.origin.x << ' ' << ray.origin.y << ' '  << ray.origin.z << '\n';
   std::cerr << "Ray Direction: " << ray.direction.x << ' ' << ray.direction.y << ' '  << ray.direction.z << '\n';
   using namespace glm;
   std::vector<int> pointsAlongRay;
   for (unsigned int i = 0; i < control.size(); i += 3)
   {
      vec3 aToP = ray.origin
         - vec3(control[i], control[i + 1], control[i + 2]);
      float distanceOnRay = -dot(aToP, ray.direction);
      vec3 pToRay = aToP + distanceOnRay * ray.direction;
      float l = length(pToRay);
      if (distanceOnRay > 0 && length(pToRay) < cutoff)
         pointsAlongRay.push_back(i);
   }

   if (pointsAlongRay.empty())
      return;

   //select closest
   int p = pointsAlongRay[0];
   int closest = p;
   vec3 point(control[p], control[p + 1], control[p + 2]);
   float minDist = distance(point, ray.origin);
   for (unsigned int i = 1; i < pointsAlongRay.size(); ++i)
   {
      p = pointsAlongRay[i];
      point = vec3(control[p], control[p + 1], control[p + 2]);
      float dist = distance(point, ray.origin);
      if (dist < minDist)
      {
         dist = minDist;
         closest = p;
      }
   }
   selectedPoint = closest / 3;
}


template <int D>
void NURBS<D>::MoveSelected(const glm::vec3& displacement)
{
   //some cameras may always emit null vectors. Why waste time recalulating?
   if (displacement == glm::vec3(0, 0, 0))
      return;
   control[3 * selectedPoint] += displacement.x;
   control[3 * selectedPoint + 1] += displacement.y;
   control[3 * selectedPoint + 2] += displacement.z;
};

template <int D>
void NURBS<D>::SetSelectedPos(const glm::vec3& pos)
{
   control[3 * selectedPoint] = pos.x;
   control[3 * selectedPoint + 1] = pos.y;
   control[3 * selectedPoint + 2] = pos.z;
}

template <int D>
glm::vec3 NURBS<D>::GetSelectedPos() const
{
   return glm::vec3(control[3 * selectedPoint],
      control[3 * selectedPoint + 1], control[3 * selectedPoint + 2]);
}