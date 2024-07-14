#include <QtTest>
#include <QtTest/QtTest>
#include "../../sound.h"

class TestApplication : public QObject
{
    Q_OBJECT

public:
    TestApplication();
    ~TestApplication();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void test_case1();
    void testCalculateFrequency();
    void testCalculateFilterCoeffs();
    bool compareDoubles(double d1, double d2, double tolerance = 1e-6);

};

bool TestApplication::compareDoubles(double d1, double d2, double tolerance) {
    return std::abs(d1 - d2) < tolerance;
}


TestApplication::TestApplication()
{

}

TestApplication::~TestApplication()
{

}

void TestApplication::testCalculateFrequency()
{
    Sound* sound = new Sound(this);
    double frequency = sound->calculateFrequency(Qt::Key_A);

    // distance verification
    //QCOMPARE(frequency, 185.000347162);
    QVERIFY(compareDoubles(frequency, 185.0, 1e-2));
    QVERIFY2(compareDoubles(sound->calculateFrequency(Qt::Key_G), 261.63), "incorrect freq"); // C4

    delete sound;
}

void TestApplication::testCalculateFilterCoeffs()
{
    Sound* sound = new Sound(this);
    std::vector<double> bCoeffs, aCoeffs;

    // Test lowpass filter
    sound->calculateFilterCoeffs(Sound::LOWPASS, 1000, 500, 44100, bCoeffs, aCoeffs);
    QVERIFY2(compareDoubles(bCoeffs[0], 0.000566843, 1e-2), "Incorrect b0 coefficient for lowpass filter");
    QVERIFY2(compareDoubles(bCoeffs[1], 0.001133687, 1e-2), "Incorrect b1 coefficient for lowpass filter");
    QVERIFY2(compareDoubles(bCoeffs[2], 0.000566843, 1e-2), "Incorrect b2 coefficient for lowpass filter");


        // Test highpass filter
        sound->calculateFilterCoeffs(Sound::HIGHPASS, 1000, 500, 44100, bCoeffs, aCoeffs);


    delete sound;
}



void TestApplication::initTestCase()
{

}

void TestApplication::cleanupTestCase()
{

}

void TestApplication::test_case1()
{

}

QTEST_APPLESS_MAIN(TestApplication)

#include "tst_testapplication.moc"
