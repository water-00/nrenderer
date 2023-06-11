#ifndef __BVHTREE_HPP__
#define __BVHREE_HPP__

#include <optional>
#include <vector>
#include <list>
#include <algorithm>
#include "scene/Scene.hpp"
#include "Bounds3.hpp"
#include "Ray.hpp"
#include "intersections/HitRecord.hpp"
#include "intersections/intersections.hpp"
#include "glm/gtc/matrix_transform.hpp"

using namespace std;

namespace Metropolis {
	class BVHNode {
	public:
		Bounds3 bound;
		vector<Bounds3> bounds;// 非叶子节点后面所有的盒
		BVHNode* left;
		BVHNode* right;
		BVHNode() {
			bound = Bounds3();
			left = nullptr;
			right = nullptr;
		}
		BVHNode(Bounds3 b, BVHNode* l, BVHNode* r) {
			bound = b;
			left = l;
			right = r;
		}
		void buildBounds(SharedScene spscene, vector<Node> objects) {
			for (int i = 0; i < objects.size(); i++) {
				auto node = objects[i];
				auto model = spscene->models[node.model];
				if (objects[i].type == Node::Type::SPHERE) {
					Mat4x4 t{ 1 };
					t = glm::translate(t, model.translation);
					bounds.push_back(Bounds3(&spscene->sphereBuffer[node.entity], t, bounds.size()));
				}
				else if (objects[i].type == Node::Type::TRIANGLE) {
					Mat4x4 t{ 1 };
					t = glm::translate(t, model.translation);
					bounds.push_back(Bounds3(&spscene->triangleBuffer[node.entity], t, bounds.size()));
				}
				else if (objects[i].type == Node::Type::PLANE) {
					Vec3 n = spscene->planeBuffer[node.entity].normal;
					Mat4x4 t{ 1 };
					t = glm::translate(t, model.translation);
					bounds.push_back(Bounds3(&spscene->planeBuffer[node.entity], t, bounds.size()));
				}
				else if (objects[i].type == Node::Type::MESH) {
					bounds.push_back(Bounds3(&spscene->meshBuffer[objects[i].entity]));
				}
			}
		}
	};

	class BVHTree {
	public:
		BVHNode* root;
		SharedScene spscene;
		inline BVHNode* build(vector<Bounds3> bound);
		vector<Bounds3> bounds;
		BVHTree() {};
		BVHTree(SharedScene spScene) {
			spscene = spScene;
			root = nullptr;
		}
		inline HitRecord Intersect(const Ray& ray, float closest);
		inline HitRecord getIntersect(const Ray& ray, BVHNode* root, float closest);
	};

	inline Bounds3 Union(const Bounds3& b1, const Bounds3& b2) {
		Bounds3 ret;
		ret.min = Min(b1.min, b2.min);
		ret.max = Max(b1.max, b2.max);
		return ret;
	}

	inline Bounds3 Union(const Bounds3& b, const Vec3& p)
	{
		Bounds3 ret;
		ret.min = Min(b.min, p);
		ret.max = Max(b.max, p);
		return ret;
	}

	inline HitRecord BVHTree::getIntersect(const Ray& ray, BVHNode* node, float closest) {
		HitRecord closestHit;
		
		Vec3 direction_inv = Vec3{ 1. / ray.direction.x, 1. / ray.direction.y, 1. / ray.direction.z };
		
		if (!node->bound.IntersectP(ray, direction_inv)) {
			closestHit = getMissRecord();
			return closestHit;
		}

		// 如果是叶子节点，求该节点对应实体与光线的交点
		if (node->left == nullptr && node->right == nullptr) {

			if (node->bound.type == Bounds3::Type::SPHERE) {
				auto hitRecord = Intersection::xSphere(ray, *node->bound.sp, 0.0001, closest, node->bound.node_id);
				if (hitRecord && hitRecord->t < closest) {
					closest = hitRecord->t;
					closestHit = hitRecord;
				}
			}
			else if (node->bound.type == Bounds3::Type::TRIANGLE) {
				auto hitRecord = Intersection::xTriangle(ray, *node->bound.tr, 0.0001, closest, node->bound.node_id);
				if (hitRecord && hitRecord->t < closest) {
					closest = hitRecord->t;
					closestHit = hitRecord;
				}
			}
			// 如果给平面建立BoundingBox的话那这里也要加上平面
			else if (node->bound.type == Bounds3::Type::PLANE) {
				auto hitRecord = Intersection::xPlane(ray, *node->bound.pl, 0.0001, closest, node->bound.node_id);
				if (hitRecord && hitRecord->t < closest) {
					closest = hitRecord->t;
					closestHit = hitRecord;
				}
			}
			return closestHit;
		}

		// 如果不是叶节点
		HitRecord hit1 = this->getIntersect(ray, node->left, closest);
		HitRecord hit2 = this->getIntersect(ray, node->right, closest);
		if (hit1 == nullopt && hit2 == nullopt) {
			return nullopt;
		}
		else if (hit1 != nullopt && hit2 == nullopt) {
			return hit1;
		}
		else if (hit2 != nullopt && hit1 == nullopt) {
			return hit2;
		}
		else {
			return hit1->t < hit2->t ? hit1 : hit2;
		}
	}

	inline HitRecord BVHTree::Intersect(const Ray& ray, float closest) {
		if (!BVHTree::root) {
			return getMissRecord();
		}
		root->bounds = bounds;
		HitRecord hit = BVHTree::getIntersect(ray, root, closest);
		return hit;
	}

	inline BVHNode* BVHTree::build(vector<Bounds3> bounds) {
		BVHNode* node = new BVHNode;
		node->bounds = bounds;
		if (node->bounds.size() == 1) {
			node->bound = node->bounds[0];
			node->left = nullptr;
			node->right = nullptr;
			// 只有在叶子节点才为object赋值
			return node;
		}
		else if (node->bounds.size() == 2) {
			node->left = build(vector{ node->bounds[0] });
			node->right = build(vector{ node->bounds[1] });
			node->bound = Union(node->left->bound, node->right->bound);
			return node;
		}
		// 如果有多个物体，则对包围盒进行BVH划分
		else {
			Bounds3 centroidBounds; // 包含所有物体重心坐标的包围盒
			for (int i = 0; i < node->bounds.size(); ++i) {
				centroidBounds = Union(centroidBounds, node->bounds[i].Centroid());
			}
			int dim = centroidBounds.maxExtent(); // 找到覆盖范围最大的坐标轴
			// 对重心坐标在该轴上的值排序
			switch (dim) {
			case 0:
				std::sort(node->bounds.begin(), node->bounds.end(), [](auto f1, auto f2) {
					return f1.Centroid().x < f2.Centroid().x;
					});
				break;
			case 1:
				std::sort(node->bounds.begin(), node->bounds.end(), [](auto f1, auto f2) {
					return f1.Centroid().y < f2.Centroid().y;
					});
				break;
			case 2:
				std::sort(node->bounds.begin(), node->bounds.end(), [](auto f1, auto f2) {
					return f1.Centroid().z < f2.Centroid().z;
					});
				break;
			}
			auto beginning = node->bounds.begin();
			auto middling = node->bounds.begin() + (node->bounds.size() / 2);
			auto ending = node->bounds.end();
			auto leftshapes = vector<Bounds3>(beginning, middling);
			auto rightshapes = vector<Bounds3>(middling, ending);

			assert(node->bounds.size() == (leftshapes.size() + rightshapes.size()));

			node->left = build(leftshapes);
			node->right = build(rightshapes);

			node->bound = Union(node->left->bound, node->right->bound);
		}
		return node;
	}
}
#endif