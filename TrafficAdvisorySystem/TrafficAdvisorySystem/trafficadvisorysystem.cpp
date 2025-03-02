#include "trafficadvisorysystem.h"

TrafficAdvisorySystem::TrafficAdvisorySystem(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TrafficAdvisorySystemClass())
{
    ui->setupUi(this);
    // 打开CSV文件
    QString csvPath = QDir(QCoreApplication::applicationDirPath()).filePath(u8"省会城市邻接矩阵.CSV");
    QString csvPath_2 = QDir(QCoreApplication::applicationDirPath()).filePath(u8"省会城市距离邻接矩阵.CSV");
    QFile file(csvPath);
    QFile file_2(csvPath_2);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::warning(this,u8"提示",file.errorString());
    }
    else
    {
        // 使用QTextStream读取文件内容
        QTextStream in(&file);
        int i = 0;
        int j = 0;
        // 逐行读取CSV文件
        while (!in.atEnd())
        {
            QString line = in.readLine();
            QStringList fields = line.split(',');
            if(i==0)
                myGraph = new Graph(fields.size() - 1, this);
            j = 0;
            // 处理每一行的数据，fields是CSV中的列数据
            for (const QString& field : fields)
            {
                // 跳过第一列
                if (j == 0)
                {
                    j++;
                    continue;
                }
                if (i == 0)
                {
                    cityName << field;
                    j++;
                }
                else
                {
                    myGraph->addCityEdge(i-1, j-1, field.toInt());
                    j++;
                    if (j > i) break;
                }
            }
            i++;
        }
        // 关闭文件
        file.close();
    }

    if (!file_2.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::warning(this, u8"提示", file_2.errorString());
    }
    else
    {
        // 使用QTextStream读取文件内容
        QTextStream in_2(&file_2);
        int i = 0;
        int j = 0;
        // 逐行读取CSV文件
        while (!in_2.atEnd())
        {
            QString line = in_2.readLine();
            // 跳过第一行
            if (i == 0)
            {
                i++;
                continue;
            }
            QStringList fields = line.split(',');
            j = 0;
            // 处理每一行的数据，fields是CSV中的列数据
            for (const QString& field : fields)
            {
                // 跳过第一列
                if (j == 0)
                {
                    j++;
                    continue;
                }
                if (j == i) break;
                else
                {
                    myGraph->addLengthEdge(i - 1, j - 1, field.toInt());
                    j++; 
                }
            }
            i++;
        }
        // 关闭文件
        file_2.close();
    }
    myGraph->optimizeLength();
   /* qDebug().noquote() << myGraph->printEdge(2);
    qDebug().noquote() << myGraph->printEdge(1);*/
    for (auto const& str : cityName)
    {
        ui->comboBox->addItem(str);
        ui->comboBox_2->addItem(str);
        ui->comboBox_5->addItem(str);
        ui->comboBox_6->addItem(str);
    }
    ui->comboBox_3->addItem(u8"迪杰斯特拉");
    ui->comboBox_3->addItem(u8"佛洛依德");
    connect(myGraph, &Graph::pathAll, this, &TrafficAdvisorySystem::on_pathAll);
}

TrafficAdvisorySystem::~TrafficAdvisorySystem()
{
    delete ui;
}


void TrafficAdvisorySystem::on_pushButtonVerify_clicked()
{
    ui->textBrowser->append(u8"功能一输出：");
    int index = cityName.indexOf(u8"武汉");
    int indexXG = cityName.indexOf(u8"香港");
    int indexAM = cityName.indexOf(u8"澳门");
    int indexHK = cityName.indexOf(u8"海口");
    int indexTB = cityName.indexOf(u8"台北");
    if (index != -1)
    {
        QString ll;
        int i;
        std::vector<int> pathL = myGraph->getSingleLength(index,1);
        std::vector<bestPath> myPath = myGraph->getBestPath();
        for (i = 0; i < pathL.size(); i++)
        {
            if (i == index ||
                i == indexAM ||
                i == indexXG ||
                i == indexHK ||
                i == indexTB
                )   continue;
            ll += QString(u8"武汉到%1中间有%2个省会(包括直辖市)").arg(cityName[i]).arg(pathL[i]-1);
            if (pathL[i] - 1 != 0)
            {
                for (auto const& path : myPath)
                {
                    if (path.src == index && path.dest == i)
                    {
                        ll += u8"，详细城市路径为：";
                        for (auto const& path_v : path.path)
                        {
                            ll += QString("%1-->").arg(cityName[path_v]);
                        }
                        ll.chop(3); // 删除最后的三个字符，即-->
                        break;
                    }
                }
            }
            else
            {
                ll += u8"，这两个城市所在省（直辖市）相邻";
            }
            ll += '\n';
            if (pathL[i] > 3)
            {
                break;
            }
        }
        if (i == pathL.size())
        {
            ui->textBrowser->append(u8"验证结果：成立！详细数据如下：");
            ui->textBrowser->append(ll);
        }
        else
        {
            ui->textBrowser->append(u8"验证结果：不成立！详细数据如下：");
            ui->textBrowser->append(ll);
        }
    }
}

void TrafficAdvisorySystem::on_pushButtonSearch_clicked()
{
    int src = ui->comboBox->currentIndex();
    int dest = ui->comboBox_2->currentIndex();
    if (src == dest)
    {
        QMessageBox::warning(this, u8"警告", u8"请将起点和终点选择不同的城市！");
        return;
    }
    QString ll;
    if (ui->comboBox_3->currentText() == u8"迪杰斯特拉")
    {
        std::vector<int> pathL = myGraph->getSingleLength(src, 2);
        std::vector<bestPath> myPath = myGraph->getBestPath();
        ll=QString(u8"功能二（迪杰斯特拉算法）输出：\n从%1到%2的最短路径为：").arg(cityName[src]).arg(cityName[dest]);
        for (auto const& path : myPath)
        {
            if (path.src == src && path.dest == dest)
            {
                for (auto const& path_v : path.path)
                {
                    ll += cityName[path_v];
                    ll += "-->";
                }
                ll.chop(3);
                break;
            }
        }
        ll += QString(u8"，距离为：%1").arg(pathL[dest]);
    }
    if (ui->comboBox_3->currentText() == u8"佛洛依德")
    {
        std::vector<std::vector<int>> pathL = myGraph->getMulLength(src, 2);
        std::vector<bestPath> myPath = myGraph->getBestPath();
        ll = QString(u8"功能二（弗洛伊德）输出：\n从%1到%2的最短路径为：").arg(cityName[src]).arg(cityName[dest]);
        for (auto const& path : myPath)
        {
            if (path.src == src && path.dest == dest)
            {
                for (auto const& path_v : path.path)
                {
                    ll += cityName[path_v];
                    ll += "-->";
                }
                ll.chop(3);
                break;
            }
        }
        ll += QString(u8"，距离为：%1").arg(pathL[dest][src]);
    }
    ui->textBrowser->append(ll);
}

void TrafficAdvisorySystem::on_pushButtonSearch_2_clicked()
{
    int src = ui->comboBox->currentIndex();
    int dest = ui->comboBox_2->currentIndex();
    if (src == dest)
    {
        QMessageBox::warning(this, u8"警告", u8"请将起点和终点选择不同的城市！");
        return;
    }
    pathA.clear();
    pathA.shrink_to_fit();
    enabledPath = true;
    int lims = ui->spinBox_2->value();
    ui->textBrowser->append(u8"功能三输出：");
    myGraph->findAllPaths(src, dest, lims+2);
    ui->textBrowser->append(QString(u8"共查询到 %1条结果！").arg(pathA.size()));
    ui->pushButtonQuickSort->setEnabled(true);
    ui->pushButtonSaveRes->setEnabled(true);
}

void TrafficAdvisorySystem::on_pushButtonSearch_3_clicked()
{
    if (ui->comboBox->currentText() == ui->comboBox_5->currentText())
        QMessageBox::warning(this, u8"警告", u8"请勿选择和起点相同的城市！");
    else if(ui->comboBox_2->currentText() == ui->comboBox_5->currentText())
        QMessageBox::warning(this, u8"警告", u8"请勿选择和终点相同的城市！");
    else
    {
        QString ll;
        int src = ui->comboBox->currentIndex();
        int through = ui->comboBox_5->currentIndex();
        int dest = ui->comboBox_2->currentIndex();
        std::vector<int> pathL = myGraph->getSingleLength(src, 2);
        std::vector<bestPath> myPath = myGraph->getBestPath();
        std::vector<int> pathL_2 = myGraph->getSingleLength(through, 2);
        std::vector<bestPath> myPath_2 = myGraph->getBestPath();
        ll = QString(u8"功能四输出：\n从%1出发，经过%2到%3的最短路径为：").arg(cityName[src]).arg(cityName[through]).arg(cityName[dest]);
        for (auto const& path : myPath)
        {
            if (path.src == src && path.dest == through)
            {
                for (auto const& path_v : path.path)
                {
                    ll += cityName[path_v];
                    ll += "-->";
                }
                break;
            }
        }
        for (auto const& path : myPath_2)
        {
            if (path.src == through && path.dest == dest)
            {
                for (auto path_v = path.path.begin(); path_v != path.path.end(); path_v++)
                {
                    if (path_v == path.path.begin()) continue;
                    ll += cityName[*path_v];
                    ll += "-->";
                }
                ll.chop(3);
                break;
            }
        }
        ll += QString(u8"，距离为：%1").arg(pathL[through]+pathL_2[dest]);
        ui->textBrowser->append(ll);
    }
}

void TrafficAdvisorySystem::on_pushButtonSearch_4_clicked()
{
    int src = ui->comboBox->currentIndex();
    int dest = ui->comboBox_2->currentIndex();
    int k = ui->spinBox->value();
    if (src == dest)
    {
        QMessageBox::warning(this, u8"警告", u8"请将起点和终点选择不同的城市！");
        return;
    }
    pathA.clear();
    pathA.shrink_to_fit();
    enabledPath = false;
    myGraph->findAllPaths(src, dest, 12);
    if (k > pathA.size())
    {
        QMessageBox::warning(this, u8"警告", QString(u8"超出范围，路径一共有%1条").arg(pathA.size()));
        return;
    }
    // 调用K短路径算法
    std::vector<std::pair<int, std::vector<int>>> paths = myGraph->kShortestPaths(src, dest, k, cityName.size());
    QString ll = u8"功能六输出：\n";
    int i = 1;
    // 输出结果
    for (const auto& path : paths)
    {
        ll += QString(u8"从%1到%2第%3短路径为：").arg(cityName[src]).arg(cityName[dest]).arg(i++);
        for (int node : path.second)
        {
            ll+= cityName[node];
            ll += "-->";
        }
        ll.chop(3);
        ll += QString(u8"，总距离为%1\n").arg(path.first);
    }
    ui->textBrowser->append(ll);
}

void TrafficAdvisorySystem::on_pushButtonSearch_5_clicked()
{
    if (ui->comboBox->currentText() == ui->comboBox_6->currentText())
        QMessageBox::warning(this, u8"警告", u8"请勿选择和起点相同的城市！");
    else if (ui->comboBox_2->currentText() == ui->comboBox_6->currentText())
        QMessageBox::warning(this, u8"警告", u8"请勿选择和终点相同的城市！");
    else
    {
        QString ll=u8"功能五输出：\n";
        int src = ui->comboBox->currentIndex();
        int pass = ui->comboBox_6->currentIndex();
        int dest = ui->comboBox_2->currentIndex();
        pathA.clear();
        pathA.shrink_to_fit();
        enabledPath = false;
        myGraph->findAllPaths(src, dest, 10);
        std::sort(pathA.begin(), pathA.end(), compareByLength);
        pathAll tempPath;
        bool isFind = false;
        for (auto const& pa : pathA)
        {
            if (std::find(pa.path.begin(), pa.path.end(), QString::number(pass)) == pa.path.end())
            {
                tempPath.length = pa.length;
                tempPath.path = pa.path;
                isFind = true;
                break;
            }
        }
   
        if (isFind)
        {
            ll += QString(u8"从%1到%2，绕过%3的最短路径为：").arg(cityName[src]).arg(cityName[dest]).arg(cityName[pass]);
            for (auto const& p : tempPath.path)
            {
                ll += QString(u8"%1-->").arg(cityName[p.toInt()]);
            }
            ll.chop(3);
            ll += QString(u8"距离为：%1").arg(tempPath.length);  
        }
        else
        {
            ll+= QString(u8"从%1到%2，绕过%3的最短路径不存在！").arg(cityName[src]).arg(cityName[dest]).arg(cityName[pass]);
        }
        ui->textBrowser->append(ll);
    }
}

void TrafficAdvisorySystem::on_pushButtonQuickSort_clicked()
{
    std::sort(pathA.begin(), pathA.end(), compareByLength);
    ui->textBrowser->clear();
    ui->textBrowser->append(u8"功能三输出：快速排序成功结果如下");
    int i = 1;
    for (auto const& p : pathA)
    {
        QString ll;
        ll += QString("%1\t").arg(i++);
        for (auto const& path : p.path)
        {
            ll += QString("%1-->").arg(cityName[path.toInt()]);
        }
        ll.chop(3);
        ll += QString(u8"，距离为：%1").arg(p.length);
        ui->textBrowser->append(ll);
    }
    QMessageBox::information(this, u8"提示", u8"快速排序完成");
    ui->pushButtonQuickSort->setEnabled(false);
}

void TrafficAdvisorySystem::on_pushButtonClear_clicked()
{
    ui->textBrowser->clear();
}

void TrafficAdvisorySystem::on_pushButtonSaveRes_clicked()
{
    QString defaultFileName = QString(u8"%1/%2到%3的路径结果.txt")
        .arg(QCoreApplication::applicationDirPath())
        .arg(ui->comboBox->currentText())
        .arg(ui->comboBox_2->currentText());
    QString filePath= QFileDialog::getSaveFileName(this, u8"保存文件", defaultFileName, u8"文本文件 (*.txt);;所有文件 (*)");
    if (!filePath.isEmpty())
    {
        QFile file(filePath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QTextStream out(&file);
            int i = 0;
            for (auto const& p : pathA)
            {
                out << QString::number(++i);
                for (auto const& pa : p.path)
                {
                    out << ',' << cityName[pa.toInt()];
                }
                out << ',' << QString::number(p.length) << "\n";
            }
            file.close();
            QMessageBox::warning(this, u8"提示", u8"文件保存成功");
            ui->textBrowser->append(u8"功能三输出：保存结果成功\n路径为：" + defaultFileName);
            ui->pushButtonSaveRes->setEnabled(false);
        }
        else
        {
            QMessageBox::warning(this, u8"警告", file.errorString());
            ui->textBrowser->append(u8"功能三输出：结果保存失败！");
        }
    }
}

void TrafficAdvisorySystem::on_pathAll(QStringList ll, int length)
{
    pathAll tempPath;
    tempPath.path = ll;
    tempPath.length = length;
    pathA.push_back(tempPath);
    if (enabledPath)
    {
        QString path;
        for (auto const& id : ll)
        {
            path += QString("%1-->").arg(cityName[id.toInt()]);
        }
        path.chop(3);
        path += QString(u8"，距离为：%1").arg(length);
        ui->textBrowser->append(path);
    }
}
