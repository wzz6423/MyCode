#pragma once

#include <iostream>
#include <cstdio>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <queue>
#include <functional>
#include "UnionFindSet.hpp"

namespace link_table {
	template<typename W>
	struct Edge {
		// int _srci;
		int _dsti; // 目标点的下标
		W _w; // 权值
		Edge<W>* _next;

		Edge(int dsti, const W& w)
			:_dsti(dsti),
			_w(w),
			_next(nullptr)
		{
		}
	};

	template<typename V, typename W, bool Direction = false>
	class Graph {
	private:
		using Edge = Edge<W>;
	public:
		Graph(const V* vertex, size_t num) {
			_vertexs.reserve(num);
			for (size_t i = 0; i < num; ++i) {
				_vertexs.push_back(vertex[i]);
				_indexMap[vertex[i]] = i;
			}
			_tables.resize(num, nullptr);
		}

		size_t GetVertexIndex(const V& v){
			auto it = _indexMap.find(v);
			if (it != _indexMap.end()) {
				return it->second;
			}
			else {
				throw std::invalid_argument("顶点不存在");
				return -1;
			}
		}

		void AddEdge(const V& src, const V& dst, const W& w) {
			size_t srci = GetVertexIndex(src);
			size_t dsti = GetVertexIndex(dst);

			Edge* eg = new Edge(dsti, w);
			eg->_next = _tables[srci];
			_tables[srci] = eg;

			if (Direction == false) {
				eg = new Edge(srci, w);
				eg->_next = _tables[dsti];
				_tables[dsti] = eg;
			}
		}

		void Print() {
			for (size_t i = 0; i < _vertexs.size(); ++i) {
				std::std::cout << "[" << i << "]" << "->" << _vertexs[i] << std::std::endl;
			}
			std::std::cout << std::std::endl;

			for (size_t i = 0; i < _tables.size(); ++i) {
				std::std::cout << _vertexs[i] << "[" << i << "]->";
				Edge* cur = _tables[i];
				while (cur) {
					std::std::cout << "[" << _vertexs[cur->_dsti] << ":" << cur->_dsti << ":" << cur->_w << "]->";
					cur = cur->_next;
				}
				std::std::cout << "nullptr" << std::std::endl;
			}
		}
	private:
		std::vector<V> _vertexs; // 顶点集合
		std::map<V, int> _indexMap; // 顶点映射下标
		std::vector<Edge*> _tables; // 邻接表
	};

	void TestGraph1()
	{
		//Graph<char, int, true> g("0123", 4);
		//g.AddEdge('0', '1', 1);
		//g.AddEdge('0', '3', 4);
		//g.AddEdge('1', '3', 2);
		//g.AddEdge('1', '2', 9);
		//g.AddEdge('2', '3', 8);
		//g.AddEdge('2', '1', 5);
		//g.AddEdge('2', '0', 3);
		//g.AddEdge('3', '2', 6);
		//g.Print();

		std::string a[] = { "张三", "李四", "王五", "赵六" };
		Graph<std::string, int, true> g1(a, 4);
		g1.AddEdge("张三", "李四", 100);
		g1.AddEdge("张三", "王五", 200);
		g1.AddEdge("王五", "赵六", 30);
		g1.Print();
	}
}

namespace matrix {
	template<typename V, typename W, W MAX_W = INT_MAX, bool Direction = false>
	class Graph {
	private:
		using Self = Graph<V, W, MAX_W, Direction>;

		struct Edge {
			size_t _srci;
			size_t _dsti;
			W _w;

			Edge(size_t srci, size_t dsti, W& w)
				:_srci(srci),
				_dsti(dsti),
				_w(w)
			{ }

			bool operator>(const Edge& e)const {
				return _w > e._w;
			}
		};
	public:
		Graph() = default;

		Graph(const V* vertex, size_t num) {
			_vertexs.reserve(num);
			for (size_t i = 0; i < num; ++i) {
				_vertexs.push_back(vertex[i]);
				_indexMap[vertex[i]] = i;
			}

			_matrix.resize(num);
			for (size_t i = 0; i < num; ++i) {
				_matrix[i].resize(num, MAX_W);
			}
		}

		size_t GetVertexIndex(const V& v) {
			auto it = _indexMap.find(v);
			if (it != _indexMap.end()) {
				return it->second;
			}
			else {
				std::std::cout << "不存在顶点: " << v << std::std::endl;
				throw std::invalid_argument("顶点不存在");
				return -1;
			}
		}

		void _AddEdge(size_t srci, size_t dsti, const W& w) {
			_matrix[srci][dsti] = w;
			// 无向图
			if (Direction == false) {
				_matrix[dsti][srci] = w;
			}
		}

		void AddEdge(const V& src, const V& dst, const W& w) {
			size_t srci = GetVertexIndex(src);
			size_t dsti = GetVertexIndex(dst);
			_AddEdge(srci, dsti, w);
		}

		void Print() {
			// 顶点
			for (size_t i = 0; i < _vertexs.size(); ++i) {
				std::std::cout << "[" << i << "]" << "->" << _vertexs[i] << std::std::endl;
			}
			std::std::cout << std::std::endl;

			// 矩阵
			// 横下标
			std::std::cout << "  ";
			for (size_t i = 0; i < _vertexs.size(); ++i) {
				printf("%4d", i);
			}
			std::std::cout << std::std::endl;

			for (size_t i = 0; i < _matrix.size(); ++i) {
				std::std::cout << i << " "; // 竖下标
				for (size_t j = 0; j < _matrix[i].size(); ++j) {
					if (_matrix[i][j] == MAX_W) {
						printf("%4c", '*');
					}
					else {
						printf("%4d", _matrix[i][j]);
					}
				}
				std::std::cout << std::std::endl;
			}
			std::std::cout << std::std::endl;

			for (size_t i = 0; i < _matrix.size(); ++i) {
				for (size_t j = 0; j < _matrix[i].size(); ++j) {
					if (i < j && _matrix[i][j] != MAX_W) {
						std::std::cout << _vertexs[i] << "->" << _vertexs[j] << ":" << _matrix[i][j] << std::std::endl;
					}
				}
			}
		}

		// 广度优先遍历
		void BFS(const V& src) {
			size_t srci = GetVertexIndex(src);

			std::queue<int> tmpq;
			std::vector<bool> visited(_vertexs.size(), false); // 记录是否被访问过

			tmpq.push(srci);
			visited[srci] = true;
			int levelSize = 1;

			size_t n = _vertexs.size();
			while (!tmpq.empty()) {
				for (size_t i = 0; i < levelSize; ++i) {
					int front = tmpq.front();
					tmpq.pop();
					std::std::cout << front << ":" << _vertexs[front] << std::std::endl;
					for (size_t i = 0; i < n; ++i) {
						if (_matrix[front][i] != MAX_W) {
							if (visited[i] == false) {
								tmpq.push(i);
								visited[i] = true;
							}
						}
					}

				}
				std::std::cout << std::std::endl;

				levelSize = tmpq.size();
			}
			std::std::cout << std::std::endl;
		}

		void _DFS(size_t srci, std::vector<bool>& visited) {
			std::std::cout << srci << ":" << _vertexs[srci] << std::std::endl;
			visited[srci] = true;

			for (size_t i = 0; i < _vertexs.size(); ++i) {
				if (_matrix[srci][i] != MAX_W && visited[i] == false) {
					_DFS(i, visited);
				}
			}
		}

		// 深度优先遍历
		void DFS(const V& src) {
			size_t srci = GetVertexIndex(src);
			std::vector<bool> visited(_vertexs.size(), false);

			_DFS(srci, visited);
		}

		// 最小生成树
		W Kruskal(Self& minTree) {
			size_t n = _vertexs.size();

			minTree._vertexs = _vertexs;
			minTree._indexMap = _indexMap;
			minTree._matrix.resize(n);
			for (size_t i = 0; i < n; ++i) {
				minTree._matrix[i].resize(n, MAX_W);
			}

			std::priority_queue<Edge, std::vector<Edge>, std::greater<Edge>> minque;
			for (size_t i = 0; i < n; ++i) {
				for (size_t j = 0; j < n; ++j) {
					if (i < j && _matrix[i][j] != MAX_W) {
						minque.push(Edge(i, j, _matrix[i][j]));
					}
				}
			}

			// 选 n - 1 条边
			int size = 0;
			W totalW = W();
			wzz::UnionFindSet ufs(n);
			while (!minque.empty()) {
				Edge min = minque.top();
				minque.pop();

				if (!ufs.Inset(min.srci, min._dsti)) {
					minTree.AddEdge(min._srci, min._dsti, min._w);
					ufs.Union(min._srci, min._dsti);
					++size;
					totalW += min._w;
				}
				// else{
					// std::cout << "构成环：";
					// std::cout << _vertexs[min._srci] << "->" << _vertexs[min._dsti] << ":" << min._w << std::endl;
				// }
			}

			if (size == n - 1) {
				return totalW;
			}
			else {
				return W();
			}
		}

		W Prim(Self& minTree, const W& src) {
			size_t srci = GetVertexIndex(src);
			size_t n = _vertexs.size();

			minTree._vertexs = _vertexs;
			minTree._indexMap = _indexMap;
			minTree._matrix.resize(n);
			for (size_t i = 0; i < n; ++i) {
				minTree._matrix[i].resize(n, MAX_W);
			}

			std::vector<bool> X(n, false);
			std::vector<bool> Y(n, true);
			X[srci] = true;
			Y[srci] = false;

			// 从 X->Y 集合中相连的边里面选出最小的边
			std::priority_queue<Edge, std::vector<Edge>, std::greater<Edge>> minque;
			// 先把 srci 所连接的边都加入堆中
			for (size_t i = 0; i < n; ++i) {
				if (_matrix[srci][i] != MAX_W) {
					minque.push(Edge(srci, i, _matrix[srci][i]));
				}
			}

			size_t size = 0;
			W totalW = W();
			while (!minque.empty()) {
				Edge min = minque.top();
				minque.pop();

				if (!X[min._dsti]) {
					minTree._AddEdge(min.srci, min.dsti, min._w);
					X[min._dsti] = true;
					Y[min._dsti] = false;
					++size;
					totalW += min._w;
					if (size == n - 1) {
						break;
					}

					for (size_t i = 0; i < n; ++i) {
						if (_matrix[min._dsti][i] != MAX_W && Y[i]) {
							minque.push(Edge(min._dsti, i, _matrix[min._dsti][i]));
						}
					}
				}
				else {
					//std::cout << "构成环:";
					//std::cout << _vertexs[min._srci] << "->" << _vertexs[min._dsti] << ":" << min._w << std::endl;
				}
			}

			if (size == n - 1) {
				return totalW;
			}
			else {
				return W();
			}
		}

		// 最短路径
		void PrintShortPath(const V& src, const std::vector<W>& dist, const std::vector<int>& pPath) {
			size_t srci = GetVertexIndex(src);
			size_t n = _vertexs.size();

			for (size_t i = 0; i < n; ++i) {
				if (i != srci) {
					std::vector<int> path;
					size_t parenti = i;

					while (parenti != srci) {
						path.push_back(parenti);
						parenti = pPath[parenti];
					}
					path.push_back(srci);
					std::reverse(path.begin(), path.end());

					for (auto& index : path) {
						std::std::cout << _vertexs[index] << "->";
					}
					std::std::cout << "权值和: " << dist[i] << std::std::endl;
				}
			}
		}

		// 顶点个数是N -> 时间复杂度：O(N^2) 空间复杂度：O(N)
		void Dijkstra(const V& src, std::vector<W>& dist, std::vector<int>& pPath) {
			size_t srci = GetVertexIndex(src);
			size_t n = _vertexs.size();
			dist.resize(n, MAX_W);
			pPath.resize(n, -1);

			dist[srci] = 0;
			pPath[srci] = srci;

			// 已经确定最短路径的顶点集合
			vector<bool> S(n, false);

			for (size_t i = 0; i < n; ++i) {
				// 选最短路径顶点且不在S更新其他路径
				int u = 0;
				W min = W();
				for (size_t j = 0; j < n; ++j) {
					if (S[i] == false && dist[i] < min) {
						u = i;
						min = dist[i];
					}
				}

				S[u] = true;
				// 松弛更新u连接顶点v  srci->u + u->j < srci->j
				for (size_t j = 0; j < n; ++j) {
					if (S[j] == false && _matrix[u][j] != MAX_W && dist[u] + _matrix[u][j] < dist[j]) {
						dist[j] = dist[u] + _matrix[u][j];
						pPath[j] = u;
					}
				}
			}
		}

		// 时间复杂度：O(N^3) 空间复杂度：O(N)
		bool BellmanFord(const V& src, std::vector<W>& dist, std::vector<int>& pPath) {
			size_t srci = GetVertexIndex(src);
			size_t n = _vertexs.size();

			// vector<W> dist,记录 srci-其他顶点 最短路径权值数组
			dist.resize(n, MAX_W);

			// vector<int> pPath 记录 srci-其他顶点 最短路径父顶点数组
			pPath.resize(n, -1);

			// 先更新srci->srci为缺省值
			dist[srci] = W();

			// 总体最多更新n轮
			for (size_t i = 0; i < n; ++i) {
				bool update = false;
				std::std::cout << "更新第 " << i << " 轮" << std::std::endl;
				for (size_t j = 0; j < n; ++j) {
					for (size_t k = 0; k < n; ++k) {
						// srci -> j + j -> k
						if (_matrix[j][k] != MAX_W && dist[i] + _matrix[j][k] < dist[k]) {
							update = true;
							std:::std::cout << _vertexs[j] << "->" << _vertexs[k] << ":" << _matrix[j][k] << std::std::endl;
							dist[k] = dist[i] + _matrix[j][k];
							pPath[k] = i;
						}
					}
				}

				// 如果这个轮次中没有更新出更短路径，那么后续轮次就不需要再走了
				if (update == false) {
					break;
				}
			}

			// 还能更新就是带负权回路
			for (size_t i = 0; i < n; ++i) {
				for (size_t j = 0; j < n; ++j) {
					// srci -> i + i ->j
					if (_matrix[i][j] != MAX_W && dist[i] + _matrix[i][j] < dist[j])
					{
						return false;
					}
				}
			}

			return true;
		}

		void FloydWarshall(std::vector<std::vector<W>>& vvDist, std::vector<std::vector<int>>& vvpPath) {
			size_t n = _vertexs.size();
			vvDist.resize(n);
			vvpPath.resize(n);

			// 初始化权值和路径矩阵
			for (size_t i = 0; i < n; ++i) {
				vvDist[i].resize(n, MAX_W);
				vvpPath[i].resize(n, -1);
			}

			// 直接相连的边更新一下
			for (size_t i = 0; i < n; ++i) {
				for (size_t j = 0; j < n; ++j) {
					if (_matrix[i][j] != MAX_W) {
						vvDist[i][j] = _matrix[i][j];
						vvpPath[i][j] = i;
					}

					if (i == j) {
						vvDist[i][j] = W();
					}
				}
			}

			// abcdef  a {} f ||  b {} c
			// 最短路径的更新i-> {其他顶点} ->j
			for (size_t k = 0; k < n; ++k) {
				for (size_t i = 0; i < n; ++i) {
					for (size_t j = 0; j < n; ++j) {
						// k 作为的中间点尝试去更新i->j的路径
						if (vvDist[i][k] != MAX_W && vvDist[k][j] != MAX_W && vvDist[i][k] + vvDist[k][j] < vvDist[i][j]) {
							vvDist[i][j] = vvDist[i][k] + vvDist[k][j];
							// 找跟j相连的上一个邻接顶点
							// 如果k->j 直接相连，上一个点就k，vvpPath[k][j]存就是k
							// 如果k->j 没有直接相连，k->...->x->j，vvpPath[k][j]存就是x
							vvpPath[i][j] = vvpPath[k][j];
						}
					}
				}

				// 打印权值和路径矩阵观察数据
				//for (size_t i = 0; i < n; ++i) {
				//	for (size_t j = 0; j < n; ++j) {
				//		if (vvDist[i][j] == MAX_W) {
				//			printf("%3c", '*');
				//		}
				//		else {
				//			printf("%3d", vvDist[i][j]);
				//		}
				//	}
				//	std::cout << std::endl;
				//}
				//std::cout << std::endl;

				//for (size_t i = 0; i < n; ++i) {
				//	for (size_t j = 0; j < n; ++j) {
				//		printf("%3d", vvpPath[i][j]);
				//	}
				//	std::cout << std::endl;
				//}
				//std::cout << "=================================" << std::endl;
			}
		}
	private: 
		std::vector<V> _vertexs; // 顶点集合
		std::map<V, int> _indexMap; // 顶点映射下标
		std::vector<std::vector<W>> _matrix; // 邻接矩阵
	};
}