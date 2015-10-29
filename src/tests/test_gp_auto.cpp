#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE gp_auto

#include <boost/test/unit_test.hpp>

#include <limbo/kernel/squared_exp_ard.hpp>
#include <limbo/mean/constant.hpp>
#include <limbo/model/gp_auto.hpp>
#include <fstream>

using namespace limbo;

Eigen::VectorXd make_v1(double x)
{
    Eigen::VectorXd v1(1);
    v1 << x;
    return v1;
}

struct Params {
    struct meanconstant {
        static Eigen::VectorXd constant() { return make_v1(0.0); };
    };

    struct gp_auto : public defaults::gp_auto {
    };
};

BOOST_AUTO_TEST_CASE(test_gp_auto)
{
    typedef kernel::SquaredExpARD<Params> KF_t;
    typedef mean::Constant<Params> Mean_t;
    typedef model::GPAuto<Params, KF_t, Mean_t> GP_t;

    GP_t gp(1, 1);
    std::vector<Eigen::VectorXd> observations = {make_v1(5), make_v1(10), make_v1(5)};
    std::vector<Eigen::VectorXd> samples = {make_v1(1), make_v1(2), make_v1(3)};

    gp.compute(samples, observations, 0.0);

    Eigen::VectorXd mu;
    double sigma;
    std::tie(mu, sigma) = gp.query(make_v1(1));
    BOOST_CHECK(abs((mu(0) - 5)) < 1);
    BOOST_CHECK(sigma < 1e-5);

    std::tie(mu, sigma) = gp.query(make_v1(2));
    BOOST_CHECK(abs((mu(0) - 10)) < 1);
    BOOST_CHECK(sigma < 1e-5);

    std::tie(mu, sigma) = gp.query(make_v1(3));
    BOOST_CHECK(abs((mu(0) - 5)) < 1);
    BOOST_CHECK(sigma < 1e-5);

    std::cout << "Params=>" << gp.kernel_function().h_params().transpose()
              << std::endl;

    std::ofstream ofs("gp.dat");
    for (double x = 0; x < 6; x += 0.05) {
        Eigen::VectorXd mu;
        double sigma;
        std::tie(mu, sigma) = gp.query(make_v1(x));
        BOOST_CHECK(gp.mu(make_v1(x)) == mu);
        BOOST_CHECK(gp.sigma(make_v1(x)) == sigma);
        ofs << x << " " << mu << " " << mu.array() - sigma << " "
            << mu.array() + sigma << std::endl;
    }
}