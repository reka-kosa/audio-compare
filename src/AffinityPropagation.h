//
//Based on the work of  zhaoyu: https://github.com/miaoerduo/affinity-propagation .
//

#ifndef _AP_AFFINITYPROPAGATION_H
#define _AP_AFFINITYPROPAGATION_H

#include <functional>
#include <vector>
#include <limits>
#include <iostream>

namespace AP {
    class AffinityPropagation {

    public:
        static void affinity_propagation(
                std::vector<int> &cluster_centers_indices,
                std::vector<int> &labels,
                std::vector< std::vector<float> > &S,
                int convergence_iter = 15,
                int max_iter = 200,
                float damping = 0.5
        );

    public:
        AffinityPropagation(float damping=0.5, int max_iter=200, int convergence_iter=15);
        void Fit(const std::vector<std::vector<float> >& in_arr);

        std::vector<int> get_labels() { return _labels; }
        void PrintLabels()
        {
            std::cout << "Lables: " << std::endl;
            for(auto item : _labels)
            {
                std::cout << item << ", ";
            }
            std::cout << std::endl;
        }

    private:
        float _damping;
        int _max_iter;
        int _convergence_iter;
        std::vector<std::vector<float>> _affinity_matrix;
        std::vector<int> _cluster_centers_indices;
        std::vector<int> _labels;
    };
}

#endif //AP_AFFINITYPROPAGATION_H
