#pragma once
#include "..\Common\DetectMemoryLeak.h"

#include "..\Common\MyMath.h"
#include <time.h>

#include "..\Common\TTransfer.h"

#include "..\Solver\TSolver.h"

#include "..\Common\TCHAR_Function.h"

#include "..\Draw\TDraw.h"

#include "..\Element\TBar.h"
#include "..\Common\DPOINT.h"
#include "..\Control\TMainWindow.h"
#include "..\Element\TShape.h"
#include "..\Solver\TExpressionTree.h"
#include "..\Solver\TVariableTable.h"
#include "TEquations.h"
#include "..\Element\TConstraintCoincide.h"
#include "..\Element\TConstraintColinear.h"
#include "..\Element\TDriver.h"
#include "..\Element\TElement.h"

#include "..\Dialog\DialogAnimation.h"

#include <iomanip>
#include <locale>
#include <string>

using namespace std;

TSolver::TSolver() {
    hwndOutput = NULL;
    Equations = NULL;
    pOS = NULL;
}

TSolver::~TSolver() {
    ClearEuqations();

    if (pOS != NULL)
        delete pOS;
}

void TSolver::SetHwnd(HWND hwnd) {
    hwndOutput = hwnd;
    if (hwnd != NULL) {
        pOS = new std::ostringstream;
        pOS->imbue(std::locale(""));
    } else {
        if (pOS != NULL)
            delete pOS;
        pOS = NULL;
    }
}

void TSolver::Outputln(const char *szFormat, ...) {
    if (pOS != NULL)
        if (szFormat != NULL && strlen(szFormat) > 0) {
            char *szBuffer = new char[strlen(szFormat) + 1024];
            va_list pArgList;
            va_start(pArgList, szFormat);
            _snprintf(szBuffer, strlen(szFormat) + 1024, szFormat, pArgList);
            va_end(pArgList);

            // 追加换行
            *pOS << szBuffer;
            *pOS << "\r\n";

            delete[] szBuffer;
        }
}

void TSolver::Output(char *szFormat, ...) {
    if (szFormat == NULL || pOS == NULL)
        return;
    char szBuffer[1024];
    va_list pArgList;
    va_start(pArgList, szFormat);
    _vsnprintf(szBuffer, sizeof(szBuffer) / sizeof(char), szFormat, pArgList);
    va_end(pArgList);

    *pOS << szBuffer;
}

int TSolver::GetIdFromVariableStr(std::string varname) {
    int i = varname.find_first_of("0123456789");
    std::string temp = varname.substr(i);
    return std::stoi(temp);
}

void TSolver::ClearConstraint() {
    Equations->RemoveTempEquations();
}

void TSolver::RecordStartDragPos(int index, DPOINT dpt) {
    RecordStartDragPos(pShape->Element[index], dpt);
}

void TSolver::RecordStartDragPos(TElement *pElement, DPOINT dpt) {
    DPOINT dptSPP = TDraw::GetRelative(dpt, pElement->dpt, pElement->angle);

    if (!IsZero(dptSPP.x, precision))
        dRelativeAngle = atan(dptSPP.y / dptSPP.x);
    else
        dRelativeAngle = 0;
}

// 添加鼠标约束
void TSolver::AddMouseConstraint(int index, DPOINT dptm) {
    AddMouseConstraint(pShape->Element[index], dptm);
}

// 添加鼠标约束
void TSolver::AddMouseConstraint(TElement *pElement, DPOINT dptm) {
    if (pElement->CanBeDragged()) {
        char temp[200];
        double xm = dptm.x;
        double ym = dptm.y;
        int id = pElement->id;

        sprintf(temp, "(%f-x%d)*sin(phi%d+%f)-(%f-y%d)*cos(phi%d+%f)", xm, id, id, dRelativeAngle, ym, id, id,
                dRelativeAngle);

        char szVar[100], szValue[100];
        sprintf(szVar, "x%d y%d phi%d", id, id, id);
        sprintf(szValue, "%f %f %f", pElement->dpt.x, pElement->dpt.y, pElement->angle);

        Equations->DefineVariable(pOS, szVar, szValue, true);
        Equations->AddEquation(pOS, temp, true);

        return;
    }
}

void TSolver::ClearEuqations() {
    if (Equations != NULL)
        delete Equations;
    Equations = new TEquations;

    // if (EquationsV != NULL)
    //	delete EquationsV;
    // EquationsV = new TEquations;

    vecStrDriver.clear();
}

void TSolver::RefreshEquations() {
    // 全部清理
    subsVar.clear();
    subsValue.clear();
    ClearOutput();
    ClearEuqations();

    // 开始
    Outputln("自由度: DOF = nc - nh = nb*3 - nh = %d*3 - %d = %d", pShape->nb, pShape->nh(), pShape->DOF());
    int i, j;
    DPOINT SiP, SjP, SiQ, SjQ;
    char buffer1[1024], buffer2[1024];

    Outputln("\r\n约束方程:");

    for (auto element : pShape->Element) {
        switch (element->eType) {
        case CONSTRAINT_COINCIDE: {
            pShape->GetSijP((TConstraintCoincide *)element, SiP, SjP, i, j);

            // 得到2个重合构件的广义坐标
            double xi, yi, phii, xj, yj, phij;
            pShape->GetCoordinateByElement(((TConstraintCoincide *)element)->pElement[0], &xi, &yi, &phii);
            pShape->GetCoordinateByElement(((TConstraintCoincide *)element)->pElement[1], &xj, &yj, &phij);

            // 定义变量及其初始值
            // sprintf(buffer1, "x%d y%d phi%d x%d y%d phi%d", i, i, i, j, j, j);
            // sprintf(buffer2, "%f %f %f %f %f %f", xi, yi, phii, xj, yj, phij);
            // Equations->DefineVariable(pOS, buffer1, buffer2);
            Equations->DefineOneVariable(pOS, "x" + to_string(i), xi, true);
            Equations->DefineOneVariable(pOS, "y" + to_string(i), yi, true);
            Equations->DefineOneVariable(pOS, "phi" + to_string(i), phii, true);
            Equations->DefineOneVariable(pOS, "x" + to_string(j), xj, true);
            Equations->DefineOneVariable(pOS, "y" + to_string(j), yj, true);
            Equations->DefineOneVariable(pOS, "phi" + to_string(j), phij, true);

            // 读入方程
            /* 推导结果：
            xi + xiP*cos(phii) - yiP*sin(phii) - xj  - xjP*cos(phij) + yjP*sin(phij)
            yi + xiP*sin(phii) + yiP*cos(phii) - yj  - xjP*sin(phij) - yjP*cos(phij)  */
            sprintf(buffer1, "x%d+%f*cos(phi%d)-%f*sin(phi%d)-x%d-%f*cos(phi%d)+%f*sin(phi%d)", i, SiP.x, i, SiP.y, i,
                    j, SjP.x, j, SjP.y, j);
            sprintf(buffer2, "y%d+%f*sin(phi%d)+%f*cos(phi%d)-y%d-%f*sin(phi%d)-%f*cos(phi%d)", i, SiP.x, i, SiP.y, i,
                    j, SjP.x, j, SjP.y, j);
            Equations->AddEquation(pOS, buffer1, false);
            Equations->AddEquation(pOS, buffer2, false);
            break;
        }
        case CONSTRAINT_COLINEAR: // 共线约束
        {
            TConstraintColinear *pColinear = (TConstraintColinear *)element;
            // 构件i
            pShape->GetSP(pColinear->pElement[0], pColinear->PointBeginIndexOfElement[0], SiP, i); // xiP,yiP
            pShape->GetSQ(pColinear->pElement[0], pColinear->PointEndIndexOfElement[0], SiQ, i);   // xiQ,yiQ

            // 构件j
            pShape->GetSP(pColinear->pElement[1], pColinear->PointBeginIndexOfElement[1], SjP, j); // xjP,yjP
            pShape->GetSQ(pColinear->pElement[1], pColinear->PointEndIndexOfElement[1], SjQ, j);   // xjQ,yjQ

            // 得到2个重合构件的广义坐标
            double xi, yi, phii, xj, yj, phij;
            pShape->GetCoordinateByElement(pColinear->pElement[0], &xi, &yi, &phii);
            pShape->GetCoordinateByElement(pColinear->pElement[1], &xj, &yj, &phij);

            // 定义变量及其初始值
            // sprintf(buffer1, "x%d y%d phi%d x%d y%d phi%d", i, i, i, j, j, j);
            // sprintf(buffer2, "%f %f %f %f %f %f", xi, yi, phii, xj, yj, phij);
            // Equations->DefineVariable(pOS, buffer1, buffer2, true);
            Equations->DefineOneVariable(pOS, "x" + to_string(i), xi, true);
            Equations->DefineOneVariable(pOS, "y" + to_string(i), yi, true);
            Equations->DefineOneVariable(pOS, "phi" + to_string(i), phii, true);
            Equations->DefineOneVariable(pOS, "x" + to_string(j), xj, true);
            Equations->DefineOneVariable(pOS, "y" + to_string(j), yj, true);
            Equations->DefineOneVariable(pOS, "phi" + to_string(j), phij, true);

            // 读入方程
            // 推导结果：
            /*
            (cos(phii)*(yiP - yiQ) + sin(phii)*(xiP - xiQ))*(xi - xj + xiP*cos(phii) - xjP*cos(phij) - yiP*sin(phii) +
            yjP*sin(phij))
            - (cos(phii)*(xiP - xiQ) - sin(phii)*(yiP - yiQ))*(yi - yj + yiP*cos(phii) - yjP*cos(phij) + xiP*sin(phii) -
            xjP*sin(phij))
            */
            sprintf(
                buffer1,
                "(cos(phi%d)*(%f-%f)+sin(phi%d)*(%f-%f))*(x%d-x%d+%f*cos(phi%d)-%f*cos(phi%d)-%f*sin(phi%d)+%f*sin(phi%d))\
																										-(cos(phi%d)*(%f-%f)-sin(phi%d)*(%f-%f))*(y%d-y%d+%f*cos(phi%d)-%f*cos(phi%d)+%f*sin(phi%d)-%f*sin(phi%d))",
                i, SiP.y, SiQ.y, i, SiP.x, SiQ.x, i, j, SiP.x, i, SjP.x, j, SiP.y, i, SjP.y, j, i, SiP.x, SiQ.x, i,
                SiP.y, SiQ.y, i, j, SiP.y, i, SjP.y, j, SiP.x, i, SjP.x, j);

            sprintf(buffer2,
                    "(cos(phi%d-phi%d)*(%f-%f)+sin(phi%d-phi%d)*(%f-%f))*(%f-%f)-(cos(phi%d-phi%d)*(%f-%f)-sin(phi%d-"
                    "phi%d)*(%f-%f))*(%f-%f)",
                    i, j, SiP.y, SiQ.y, i, j, SiP.x, SiQ.x, SjP.x, SjQ.x, i, j, SiP.x, SiQ.x, i, j, SiP.y, SiQ.y, SjP.y,
                    SjQ.y);

            Equations->AddEquation(pOS, buffer1, false);
            Equations->AddEquation(pOS, buffer2, false);
            break;
        }
        case ELEMENT_SLIDEWAY:
        case ELEMENT_FRAMEPOINT: {
            int id = element->id;

            subsVar.push_back("x" + to_string(id));
            subsVar.push_back("y" + to_string(id));
            subsVar.push_back("phi" + to_string(id));
            subsValue.push_back(element->dpt.x);
            subsValue.push_back(element->dpt.y);
            subsValue.push_back(element->angle);
            // Solve时，建立Jacobian会替换掉
            break;
        }
        case DRIVER: {
            TDriver *pDriver = (TDriver *)element;
            std::string s;
            s += pDriver->sExprLeft + "-(" + pDriver->sExprRight + ")";
            vecStrDriver.push_back(s);
            break;
        }
        }
    }

    if (Equations->GetEquationsCount() == 0)
        Outputln("\r\n无约束。\r\n");

    if (pShape->DOF() == 1)
        Outputln("\r\n可以拖动。");
    else if (pShape->DOF() > 1)
        Outputln("\r\n欠约束。");

    if (pOS != NULL)
        RefreshWindowText();
}

void TSolver::ClearOutput() {
    if (pOS != NULL) {
        delete pOS;
        pOS = new std::ostringstream;
        pOS->imbue(std::locale(""));
    }
}

// 必须使用了本函数才刷新Edit内容
void TSolver::RefreshWindowText() {
    if (hwndOutput != NULL) {
        SetWindowText(hwndOutput, ((Ostringstream *)pOS)->str().c_str());

        // SetFocus(Edit.m_hWnd);

        int len = ::GetWindowTextLength(hwndOutput);
        ::PostMessage(hwndOutput, EM_SETSEL, len, len);
        PostMessage(hwndOutput, EM_SCROLLCARET, 0, 0);
    }
}

// 将Shape的Element设置为VariableTable定义的数值
void TSolver::SetElementDisplacement(const TVariableTable &VariableTable) {
    for (size_t i = 0; i < VariableTable.VariableTable.size(); i++) {
        int id = 0;
        try {
            id = GetIdFromVariableStr(VariableTable.VariableTable[i]);
        } catch (std::exception &err) {
            // var is not a number
            continue;
        }
        TElement *element = pShape->GetElementById(id);
        const double &data = VariableTable.VariableValue[i];

        switch (VariableTable.VariableTable[i][0]) {
        case 'x':
            element->SetX(data);
            break;
        case 'y':
            element->SetY(data);
            break;
        case 'p':
            element->SetPhi(data); // MakeIn2Pi
            break;
        default:
            continue;
        }
    }
}

// 区分x y phi，按照变量表的顺序，将vecpValue链接到元素的值
void TSolver::LinkpValue(std::vector<double *> &vecpValue) {
    vecpValue.clear();
    TVariableTable &VariableTable = Equations->VariableTable;
    for (size_t i = 0; i < VariableTable.VariableTable.size(); i++) {
        int id = GetIdFromVariableStr(VariableTable.VariableTable[i]);
        TElement *element = pShape->GetElementById(id);

        switch (VariableTable.VariableTable[i][0]) {
        case 'x':
            vecpValue.push_back(&(element->dpt.x));
            break;
        case 'y':
            vecpValue.push_back(&(element->dpt.y));
            break;
        case 'p':
            vecpValue.push_back(&(element->angle));
            break;
        default:
            continue;
        }
    }
}

void TSolver::GetResult(std::vector<double> &vecResult) {
    vecResult = Equations->VariableTable.VariableValue;
}

// 求解
void TSolver::Solve() {
    clock_t start, stop; // clock_t是clock()函数返回类型
    double duration;

    start = clock();

    SubsFramePoint();
    Equations->SimplifyEquations(pOS);
    Equations->BuildJacobian(pOS);
    Equations->SolveEquations(pOS); // 解方程

    if (Equations->hasSolved) // 解出
    {
        SetElementDisplacement(Equations->VariableTable);
    }

    stop = clock();
    duration = ((double)(stop - start)) / CLK_TCK;

    Outputln("\r\n耗时 %f s", duration);

    if (pOS != NULL)
        RefreshWindowText();
}

void TSolver::SubsFramePoint() {
    if (!subsVar.empty() && !subsValue.empty())
        Equations->Subs(pOS, subsVar, subsValue);
}

// 求解之前需要调用SubsFramePoint
// 返回是否求解成功
bool TSolver::Solve(double t) {
    if (pOS) {
        std::string s(30, '=');
        *pOS << s << " Solve(t=" << t << " " << s << "\r\n";
    }

    Equations->RemoveTempEquations();

    Equations->DefineOneVariable(pOS, "t", t, true);

    SubsFramePoint();

    // 加入驱动方程
    for (auto StrDriver : vecStrDriver)
        Equations->AddEquation(pOS, StrDriver, true);

    Equations->BuildEquationsV(pOS); // 此时t还在变量组内
    Equations->BuildEquationsA_Phitt(pOS);

    Equations->Subs(pOS, "t", t); // 方程组代入t
    // Equations->SimplifyEquations(pOS);

    Equations->BuildVariableTableV(pOS);
    Equations->BuildVariableTableA(pOS);

    Equations->BuildJacobian(pOS);

    // 解出位移方程
    Equations->SolveEquations(pOS);

    if (Equations->hasSolved) {
        // 解出速度方程
        Equations->SubsV(pOS, "t", t);
        Equations->SolveEquationsV(pOS);

        // 解出加速度方程
        Equations->SubsA(pOS, "t", t);
        Equations->SolveEquationsA(pOS);

        // 设置位移
        SetElementDisplacement(Equations->VariableTable);
    }
    RefreshWindowText();

    return Equations->hasSolved;
}

void TSolver::GetMesureResult(std::vector<DialogAnimation::TListBoxItem> &vecItems, std::vector<int> &vecIndex) {
    for (size_t i = 0; i < vecItems.size(); ++i) {
        TVariableTable *pVarTable;
        switch (vecItems[i].type) {
            using namespace DialogAnimation;
        case D: {
            pVarTable = &(Equations->VariableTableUnsolved); // 指向VariableTable

            DPOINT dptAb = vecItems[i].pElement->GetAbsolutePointByIndex(vecItems[i].index_of_point);
            switch (vecItems[i].value_type) {
            case X:
                vecItems[i].data.push_back(dptAb.x);
                break;
            case Y:
                vecItems[i].data.push_back(dptAb.y);
                break;
            case PHI:
                vecItems[i].data.push_back(pVarTable->VariableValue[vecIndex[i]]);
                break;
            }
            break;
        }
        case V:
            pVarTable = &(Equations->VariableTableV); // 指向VariableTableV
            vecItems[i].data.push_back(pVarTable->VariableValue[vecIndex[i]]);
            break;
        case A:
            pVarTable = &(Equations->VariableTableA); // 指向VariableTableA
            vecItems[i].data.push_back(pVarTable->VariableValue[vecIndex[i]]);
            break;
        }
    }
}

// 以未解出变量表为顺序依据，调用前必须SubsFramePoint
void TSolver::LinkMesureResult(const std::vector<DialogAnimation::TListBoxItem> &vecItems, std::vector<int> &vecIndex) {
    std::string temp;
    for (auto &Item : vecItems) {
        switch (Item.value_type) {
            using namespace DialogAnimation;
        case X:
            temp = "x";
            break;
        case Y:
            temp = "y";
            break;
        case PHI:
            temp = "phi";
            break;
        }
        temp += to_string(Item.id);
        // 至此得到变量名

        // 位移表和速度表项目顺序一致，得到变量索引
        TVariableTable *pVarTable = &(Equations->VariableTableUnsolved); // 以未解出变量表顺序为准

        auto it = find(pVarTable->VariableTable.begin(), pVarTable->VariableTable.end(), temp);
        if (it != pVarTable->VariableTable.end()) {
            vecIndex.push_back(it - pVarTable->VariableTable.begin());
        }
    }
}

void TSolver::Demo() {
    // Outputln(Equations->VariableTable.Define(true, "x1 y1 phi1 x2 y2 phi2 l t","0 0 0 2.3 0 0 1.3 0"));
    // Outputln(Equations->AddEquation(true, "x1", false));
    // Outputln(Equations->AddEquation(true, "y1", false));
    // Outputln(Equations->AddEquation(true, "y2", false));
    // Outputln(Equations->AddEquation(true, "x1-x2+cos(phi1)+l*sin(phi2)", false));
    // Outputln(Equations->AddEquation(true, "y1-y2+sin(phi1)-l*cos(phi2)", false));
    // Outputln(Equations->AddEquation(true, "phi1-t", false));
    // Outputln(Equations->BuildJacobian(true, "l t", "1.3 0"));

    /*
    Outputln(Equations->DefineVariable(true, "x2 phi2 z t l", "0 0 0 0 0"));
    Outputln(Equations->AddEquation(true, "-x2+1+1.3*sin(phi2)*l", false));
    Outputln(Equations->AddEquation(true, "-1.3*(-phi2/(5+4))-20", false));
    Outputln(Equations->AddEquation(true, "2*sin(z)+0.5*cos(z)+t", false));
    Outputln(Equations->AddEquation(true, "t-0.3", false));
    Outputln(Equations->Subs(true, "l", "1.3"));
    Outputln(Equations->SimplifyEquations(true));
    Outputln(Equations->BuildJacobian(true));
    Outputln(Equations->SolveEquations(true));*/

    Equations->RemoveTempEquations();

    Equations->DefineOneVariable(pOS, "t", 0.1);
    Equations->AddEquation(pOS, "phi2-ln(t)", true);
    Equations->Subs(pOS, subsVar, subsValue);

    Equations->BuildEquationsV(pOS);       // 此时t还在变量组内
    Equations->BuildEquationsA_Phitt(pOS); //

    Equations->Subs(pOS, "t", 0.1);
    // Equations->SimplifyEquations(pOS);

    Equations->BuildVariableTableV(pOS); //
    Equations->BuildVariableTableA(pOS); //

    Equations->BuildJacobian(pOS);

    // 解出位移方程
    Equations->SolveEquations(pOS);

    if (Equations->hasSolved) {
        // 解出速度方程
        Equations->SubsV(pOS, "t", 0.1); //
        Equations->SolveEquationsV(pOS); //

        // 解出加速度方程
        Equations->SubsA(pOS, "t", 0.1); //
        Equations->SolveEquationsA(pOS);

        // 设置位移
        SetElementDisplacement(Equations->VariableTable);
    }
    RefreshWindowText();

    // EquationsV->RemoveTempEquations();
    // EquationsV->DefineVariable(pOS, "t", "0");
    // EquationsV->AddEquation(pOS, "phi2-sin(t)", true);
    // EquationsV->Subs(pOS, subsVar, subsValue);
    // EquationsV->BuildJacobian(pOS);
    // for (auto pEqua : Equations->Equations)
    //{
    //	Outputln(pEqua->Diff("t", 1, true));
    //	Outputln(pEqua->Simplify(true));
    // }
    // Equations->SolveEquations(pOS);

    // 建立ExpressionTree
    // TExpressionTree ex;
    // TVariableTable VarTable;
    // Outputln(VarTable.Define(true,"  x   y z   "));
    // Outputln(ex.LinkVariableTable(&VarTable));
    // Outputln(ex.Read("sqrt(x/y)", true));
    // Outputln(ex.Simplify(true));
    // Outputln(ex.Diff("x", 1, true));
    // Outputln(ex.Simplify(true));
    // Outputln(ex.Subs("x y", " y 0.1 ", true));
    // Outputln(ex.Simplify(true));
    // Outputln("%f", ex.Value(true));
}