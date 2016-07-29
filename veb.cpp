#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
typedef unsigned long long ull;
typedef unsigned int u32;

class VEB {
	public:
		VEB() {}
		virtual bool insert(ll key) = 0;
		virtual bool remove(ll key) = 0;
		virtual ll next(ll key) = 0;
		virtual ll prev(ll key) = 0;
		virtual bool isEmpty() = 0;
		virtual ll umin() = 0;
		virtual ll umax() = 0;
};

VEB* createVEB(int exp);

class RecursiveVEB : public VEB {
	int numbits;
	ll mn = 1LL<<62, mx = -1;
	VEB *aux = NULL;
	unordered_map<u32, VEB*> children;
	
	public:
	RecursiveVEB(int exp) { numbits = exp >> 1; }
	bool insert(ll key) {
		u32 base = key >> numbits;
		if (aux == NULL) aux = createVEB(numbits);
		aux->insert(base);

		auto it = children.find(base);
		if (it == children.end()) it = children.insert({base, createVEB(numbits)}).first;
		mn = min(mn, key);
		mx = max(mx, key);
		return it->second->insert(key & ((1 << numbits) - 1));
	}
	bool remove(ll key) {
		if (isEmpty()) return false; // if not empty, aux must exist

		u32 base = key >> numbits, pos = key & ((1 << numbits) - 1);
		auto it = children.find(base);
		if (it == children.end()) return false;

		bool res = it->second->remove(pos);
		if (it->second->isEmpty()) {
			children.erase(base);
			aux->remove(base);
		}

		if (key == mn && key == mx)
			mn = 1LL<<63, mx = -1;
		else {
			if (key == mn) mn = (aux->umin() << numbits) + children[aux->umin()]->umin();
			if (key == mx) mx = (aux->umax() << numbits) + children[aux->umax()]->umax();
		}
		return res;
	}
	ll next(ll key) {
		if (key >= mx || isEmpty()) return -1;
		if (key < mn) return mn;
		u32 base = key >> numbits, pos = key & ((1 << numbits) - 1);
		auto it = children.find(base);
		if (it == children.end() || key >= it->second->umax()) {
			u32 nextBase = aux->next(base);
			return ((key ^ pos) | children[nextBase]->umin()) + ((nextBase - base) << numbits);
		}
		return (key ^ pos) | it->second->next(pos);
	}
	ll prev(ll key) {
		if (key <= mn || isEmpty()) return -1;
		if (key > mx) return mx;
		u32 base = key >> numbits, pos = key & ((1 << numbits) - 1);
		auto it = children.find(base);
		if (it == children.end() || key <= it->second->umin()) {
			u32 prevBase = aux->prev(base);
			return ((key ^ pos) | children[prevBase]->umax()) - ((base - prevBase) << numbits);
		}
		return (key ^ pos) | it->second->prev(pos);
	}
	inline bool isEmpty() { return mn > mx; }
	inline ll umin() { return mn; }
	inline ll umax() { return mx; }
};

class BitVEB : public VEB {
	int mn = 64, mx = 0;
	ull data = 0;

	public:
	bool insert(ll key) {
		if ((data >> key) & 1) return false;
		data |= 1<<key;
		mn = min(mn, (int)key);
		mx = max(mx, (int)key);
		return true;
	}
	bool remove(ll key) {
		if ((data >> key) ^ 1) return false;
		data ^= (1 << key);
		if (key == mn && key == mx)
			mn = 64, mx = -1;
		else {
			if (key == mn) mn = next(key);
			if (key == mx) mx = prev(key);
		}
		return true;
	}
	ll next(ll key) {
		if (key >= mx || isEmpty()) return -1;
		if (key < mn) return mn;
		ll tmp = data >> ++key;
		return key + __builtin_ctzll(tmp&-tmp);
	}

	ll prev(ll key) {
		if (key <= mn || isEmpty()) return -1;
		if (key > mx) return mx;
		return 63 ^ __builtin_clzll(data & ((1<<key) - 1));
	}

	inline bool isEmpty() { return data == 0; }
	inline ll umin() { return mn; }
	inline ll umax() { return mx; }
};

VEB* createVEB(int exp) {
	if (exp > 6) return new RecursiveVEB(exp);
	return new BitVEB();
}

int main() {
	VEB *v = createVEB(48);
	v->insert(4);
	v->insert(23);
	v->insert(7);
	cout << v->umin() << ' ' << v->umax() << ' ' << v->next(5) << ' ' << v->prev(5) << '\n';
}
