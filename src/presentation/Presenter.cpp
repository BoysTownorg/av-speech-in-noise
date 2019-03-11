#include "Presenter.h"

namespace presentation {
    Presenter::Presenter(Model *model, View *view) :
        testSetup{view->testSetup()},
        tester{view->tester()},
        model{model},
        view{view}
    {
        testSetup.setParent(this);
        tester.setParent(this);
        view->subscribe(this);
    }

    void Presenter::run() {
        view->eventLoop();
    }

    void Presenter::newTest() {
        testSetup.run();
    }

    void Presenter::openTest() {
        tester.run();
    }

    void Presenter::closeTest() {
        if (view->showConfirmationDialog() == View::DialogResponse::cancel)
            return;
        tester.close();
    }
    
    void Presenter::initializeTest() {
        try {
            testSetup.submitRequest(model);
            testSetup.close();
            tester.run();
        } catch (const BadInput &e) {
            view->showErrorMessage(e.what());
        } catch (const Model::RequestFailure &e) {
            view->showErrorMessage(e.what());
        }
    }
    
    void Presenter::playTrial() { 
        model->playTrial();
    }

    Presenter::TestSetup::TestSetup(View::TestSetup *view) :
        view{view}
    {
        view->subscribe(this);
    }

    void Presenter::TestSetup::confirmTestSetup() {
        parent->initializeTest();
    }
    
    void Presenter::TestSetup::setParent(presentation::Presenter *p) {
        parent = p;
    }
    
    void Presenter::TestSetup::run() { 
        view->show();
    }
    
    void Presenter::TestSetup::close() { 
        view->hide();
    }
    
    void Presenter::TestSetup::submitRequest(presentation::Model *model) {
        Model::TestParameters p;
        try {
            p.maskerLevel_dB_SPL =
                std::stoi(view->maskerLevel_dB_SPL());
            p.signalLevel_dB_SPL =
                std::stoi(view->signalLevel_dB_SPL());
        }
        catch (const std::invalid_argument &) {
            throw BadInput{"'" + view->maskerLevel_dB_SPL() + "' is not a valid masker level."};
        }
        p.maskerFilePath = view->maskerFilePath();
        p.stimulusListDirectory =
            view->stimulusListDirectory();
        p.subjectId = view->subjectId();
        p.testerId = view->testerId();
        p.condition =
            view->condition() == "Auditory-only"
            ? Model::TestParameters::Condition::auditoryOnly
            : Model::TestParameters::Condition::audioVisual;
        model->initializeTest(std::move(p));
    }
    
    
    Presenter::Tester::Tester(View::Tester *view) :
        view{view}
    {
        view->subscribe(this);
    }
    
    void Presenter::Tester::run() {
        view->show();
    }

    void Presenter::Tester::close() {
        view->hide();
    }
    
    void Presenter::Tester::setParent(presentation::Presenter *p) {
        parent = p;
    }

    void Presenter::Tester::playTrial() {
        parent->playTrial();
    }
}
