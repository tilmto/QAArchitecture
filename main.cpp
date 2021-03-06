#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <ctime>
#include <sys/types.h>
#include <dirent.h>

#define infinity 10000000
#define Readahead 4

using namespace std;

///////////////  Beginning: Original Hardware Based on Swap  ///////////////////

class HardwareA
{
public:
    int qubitNum;

    int edgeNum;

    bool isUniDirection;

    vector<vector<bool>> archMatrix;

    vector<vector<int>> distMatrix;

    vector<vector<int>> routeMatrix;

    vector<int> outdeg;

    vector<int> mapArray;

    HardwareA(string hwname,bool isUniDirection);

    int GetQNum();

    int GetENum();

    void GetArch(string hwname);

    void changeArch();

    void PrintArchMatrix();

    void Floyd();

    void VerifyRouteMatrix();

    void PrintRouteMatrix();

    void PrintPath(int i,int j);

    void PrintMap();

    void InitMap(vector<vector<int>> seq);

    int Alloc(vector<vector<int>> seq);

};


HardwareA::HardwareA(string hwname,bool isUniDirection=true)
{
    this->isUniDirection=isUniDirection;

    GetArch(hwname);

    Floyd();

    cout << "Complete analyzing hardware." << endl;
}


void HardwareA::GetArch(string hwname)
{
    int adjIndex,i;

    ifstream is(hwname,ios::in);
    if(!is)
    {
        cout << "Cannot Open Hardware File." << endl;
        exit(1);
    }

    qubitNum=0;
    edgeNum=0;

    while(!is.eof())
    {
        is>>adjIndex;
        if(adjIndex == -1)
            qubitNum++;
    }
    qubitNum--;

    for(i=0; i<qubitNum; i++)
    {
        archMatrix.push_back(vector<bool>(qubitNum,false));
        distMatrix.push_back(vector<int>(qubitNum));
        routeMatrix.push_back(vector<int>(qubitNum));
        outdeg.push_back(0);
    }

    mapArray.resize(qubitNum);

    i=0;
    is.clear();
    is.seekg(0,ios::beg);

    while(i<qubitNum && !is.eof())
    {
        is>>adjIndex;
        if(adjIndex==-1)
        {
            archMatrix[i][i]=true;
            i++;
        }

        else
        {
            archMatrix[i][adjIndex]=true;
            outdeg[i]++;
            edgeNum++;
        }
    }

    is.close();
}


void HardwareA::changeArch()
{
    int cqubit,squbit;

    srand((int)time(0));

    cqubit=rand()%qubitNum;

    if(cqubit>=1 && rand()%10<7)
    {
        if(archMatrix[cqubit][cqubit-1])
        {
            archMatrix[cqubit][cqubit-1]=false;
            archMatrix[cqubit-1][cqubit]=true;
        }

        else
        {
            archMatrix[cqubit][cqubit-1]=true;
            archMatrix[cqubit-1][cqubit]=false;
        }
    }

    if(cqubit%4<3 && rand()%10<7)
    {
        if(archMatrix[cqubit][cqubit+1])
        {
            archMatrix[cqubit][cqubit+1]=false;
            archMatrix[cqubit+1][cqubit]=true;
        }

        else
        {
            archMatrix[cqubit][cqubit+1]=true;
            archMatrix[cqubit+1][cqubit]=false;
        }
    }

    if(cqubit-4>=0 && rand()%10<7)
    {
        if(archMatrix[cqubit][cqubit-4])
        {
            archMatrix[cqubit][cqubit-4]=false;
            archMatrix[cqubit-4][cqubit]=true;
        }

        else
        {
            archMatrix[cqubit][cqubit-4]=true;
            archMatrix[cqubit-4][cqubit]=false;
        }
    }

    if(cqubit+4<=15 && rand()%10<7)
    {
        if(archMatrix[cqubit][cqubit+4])
        {
            archMatrix[cqubit][cqubit+4]=false;
            archMatrix[cqubit+4][cqubit]=true;

        }

        else
        {
            archMatrix[cqubit][cqubit+4]=true;
            archMatrix[cqubit+4][cqubit]=false;
        }
    }

}


void HardwareA::PrintArchMatrix()
{
    cout << "Architecture Matrix:" << endl;
    for(int i=0; i<qubitNum; i++)
        for(int j=0; j<qubitNum; j++)
        {
            cout << archMatrix[i][j] << " ";
            if(j==qubitNum-1)
                cout << endl;
        }
}


void HardwareA::Floyd()
{
    int i,j,k;
    for(i=0; i<qubitNum; i++)
        for(j=0; j<qubitNum; j++)
        {
            if(!archMatrix[i][j] && !archMatrix[j][i])
            {
                distMatrix[i][j]=infinity;
                routeMatrix[i][j]=-1;
            }

            else
            {
                distMatrix[i][j]=1;
                routeMatrix[i][j]=j;
            }
        }

    for(k=0; k<qubitNum; k++)
        for(i=0; i<qubitNum; i++)
            for(j=0; j<qubitNum; j++)
                if(distMatrix[i][j]>distMatrix[i][k]+distMatrix[k][j])
                {
                    distMatrix[i][j]=distMatrix[i][k]+distMatrix[k][j];
                    routeMatrix[i][j]=routeMatrix[i][k];
                }

    for(i=0; i<qubitNum; i++)
        for(j=0; j<qubitNum; j++)
        {
            if(archMatrix[i][j] && i!=j)
                for(k=0; k<qubitNum; k++)
                {
                    if(archMatrix[j][k] && j!=k)
                    {
                        routeMatrix[i][k]=j;
                        routeMatrix[k][i]=j;
                    }
                }
        }

    VerifyRouteMatrix();
}



void HardwareA::VerifyRouteMatrix()
{
    for(int i=0; i<qubitNum; i++)
        for(int j=0; j<qubitNum; j++)
            if(routeMatrix[i][j]==-1)
            {
                cout << "Not fully connected architecture." << endl;
                exit(1);
            }
}


void HardwareA::PrintRouteMatrix()
{
    cout << "Route Matrix:" << endl;
    for(int i=0; i<qubitNum; i++)
        for(int j=0; j<qubitNum; j++)
        {
            cout << routeMatrix[i][j] << " ";
            if(j==qubitNum-1)
                cout << endl;
        }
}


void HardwareA::PrintPath(int i,int j)
{
    int next=routeMatrix[i][j];
    if(next==-1)
        cout << "No Path between " << i << " and "<< j << endl;
    else
    {
        cout << "Path from " << i << " to " << j << ": " << i << " ";
        while(next!=j)
        {
            cout << next << " ";
            next=routeMatrix[next][j];
        }
        cout << j << endl;
    }
}

int HardwareA::GetQNum()
{
    return qubitNum;
}


int HardwareA::GetENum()
{
    return edgeNum;
}


void HardwareA::InitMap(vector<vector<int>> seq)
{
    int i;
    unsigned int j;
    vector<int> freq(qubitNum,0);
    vector<int> sortFreq(1,0);
    vector<int> sortOutDeg(1,0);

    for(j=0; j<seq.size(); j++)
        if(seq[j][0]>=0)
            freq[seq[j][0]]++;

    for(i=1; i<qubitNum; i++)
        for(j=0; j<sortFreq.size(); j++)
        {
            if(freq[i]>freq[sortFreq[j]])
            {
                sortFreq.insert(sortFreq.begin()+j,i);
                break;
            }

            if(j==sortFreq.size()-1)
            {
                sortFreq.push_back(i);
                break;
            }
        }

    for(i=1; i<qubitNum; i++)
        for(j=0; j<sortOutDeg.size(); j++)
        {
            if(outdeg[i]>outdeg[sortOutDeg[j]])
            {
                sortOutDeg.insert(sortOutDeg.begin()+j,i);
                break;
            }

            if(j==sortOutDeg.size()-1)
            {
                sortOutDeg.push_back(i);
                break;
            }
        }

    for(i=0; i<qubitNum; i++)
        mapArray[sortOutDeg[i]]=sortFreq[i];

}


void HardwareA::PrintMap()
{
    int i;
    cout << "Physical qubits: ";
    for(i=0; i<qubitNum; i++)
        cout << i << " ";
    cout << endl;
    cout << "Pseudo   qubits: ";
    for(i=0; i<qubitNum; i++)
        cout << mapArray[i] << " ";
    cout << endl;
}


int HardwareA::Alloc(vector<vector<int>> seq)
{
    unsigned int i;
    int j,temp,current,next,dest,cost=0;

    for(i=0; i<seq.size(); i++)
        if(seq[i][0]>=0)
        {
            for(j=0; j<qubitNum; j++)
            {
                if(mapArray[j]==seq[i][1])
                    current=j;

                if(mapArray[j]==seq[i][0])
                    dest=j;
            }

            next=routeMatrix[current][dest];

            while(next!=dest)
            {
                temp=mapArray[current];
                mapArray[current]=mapArray[next];
                mapArray[next]=temp;

                cost=cost+7;

                current=next;
                next=routeMatrix[current][dest];
            }

            if(archMatrix[current][next])
                cost++;
            else
                cost=cost+5;
        }

    return cost;
}

///////////////  Ending: Original Hardware Based on Swap  ///////////////////

int frac(int n);

////////////////////  Beginning: Hardware with bridge  //////////////////////

class HardwareB:public HardwareA
{
public:
    HardwareB(string hwname,bool isUniDirection);

    void InitMap(vector<vector<int>> seq);

    int Alloc(vector<vector<int>> seq);

    void SubAlloc(vector<vector<int>> worklist,vector<int> mapArray,vector<bool> hadamard,vector<bool>& minhadamard,vector<int>& minmap,int& mincost);
};

HardwareB::HardwareB(string hwname,bool isUniDirection=true):HardwareA(hwname,isUniDirection) {}

void HardwareB::InitMap(vector<vector<int>> seq)
{
    int i;
    unsigned int j;
    vector<int> freq(qubitNum,0);
    vector<int> sortFreq(1,0);
    vector<int> sortOutDeg;

    for(j=0; j<seq.size(); j++)
        if(seq[j][0]>=0)
            freq[seq[j][1]]++;

    for(i=1; i<qubitNum; i++)
        for(j=0; j<sortFreq.size(); j++)
        {
            if(freq[i]>freq[sortFreq[j]])
            {
                sortFreq.insert(sortFreq.begin()+j,i);
                break;
            }

            if(j==sortFreq.size()-1)
            {
                sortFreq.push_back(i);
                break;
            }
        }

        sortOutDeg.push_back(6);
        sortOutDeg.push_back(5);
        sortOutDeg.push_back(10);
        sortOutDeg.push_back(9);
        sortOutDeg.push_back(7);
        sortOutDeg.push_back(2);
        sortOutDeg.push_back(1);
        sortOutDeg.push_back(4);
        sortOutDeg.push_back(13);
        sortOutDeg.push_back(14);
        sortOutDeg.push_back(8);
        sortOutDeg.push_back(11);
        sortOutDeg.push_back(15);
        sortOutDeg.push_back(12);
        sortOutDeg.push_back(0);
        sortOutDeg.push_back(3);

    for(i=0; i<qubitNum; i++)
        mapArray[sortOutDeg[i]]=sortFreq[i];

}


int HardwareB::Alloc(vector<vector<int>> seq)
{
    unsigned int i,j;
    int m,n,seqLen,permuteNum,mincost,record,cnt,totalcost=0;
    bool flag=false;
    vector<vector<int>> worklist;
    vector<int> minmap;
    vector<int> temp;
    vector<bool> hadamard(qubitNum,false);
    vector<bool> minhadamard;
    vector<bool> seqitem(seq.size(),true);
    vector<bool> vacant(qubitNum,true);

    record=seq.size();
    cnt=0;

    for(i=0; i<seq.size();i++)
    {
        if(flag)
            cnt++;

        if(seq[i][0]==-1 && seqitem[i])
        {
            if(vacant[seq[i][1]])
            {
                for(j=0; j<qubitNum; j++)
                {
                    if(mapArray[j]==seq[i][1])
                        break;
                }

                totalcost++;

                hadamard[j]=false;

                seqitem[i]=false;
            }

            else if(record>i)
            {
                record=i;
                flag=true;
            }
        }

        else if(seq[i][0]==-2 && seqitem[i])
        {
            if(vacant[seq[i][1]])
            {
                for(j=0; j<qubitNum; j++)
                {
                    if(mapArray[j]==seq[i][1])
                        break;
                }

                if(hadamard[j])
                {
                    totalcost--;
                    hadamard[j]=false;
                }

                else
                {
                    totalcost++;
                    hadamard[j]=true;
                }

                seqitem[i]=false;
            }

            else if(record>i)
            {
                record=i;
                flag=true;
            }
        }

        else if(seq[i][0]>=0 && seqitem[i])
        {
            if(vacant[seq[i][0]] && vacant[seq[i][1]])
            {
                worklist.push_back(seq[i]);
                vacant[seq[i][0]]=false;
                vacant[seq[i][1]]=false;
                seqitem[i]=false;
            }

            else if(record>i)
            {
                record=i;
                flag=true;
            }
        }

        if(cnt>=Readahead || i==seq.size()-1)
        {
            if(worklist.size())
            {
                mincost=infinity;
                seqLen=worklist.size();

                if(seqLen==1)
                    SubAlloc(worklist,mapArray,hadamard,minhadamard,minmap,mincost);

                else
                {
                    permuteNum=frac(seqLen);
                    m=0;
                    while(m<permuteNum)
                    {
                        for(n=seqLen-1; n>0; n--)
                        {
                            temp=worklist[n];
                            worklist[n]=worklist[n-1];
                            worklist[n-1]=temp;
                            m++;
                            SubAlloc(worklist,mapArray,hadamard,minhadamard,minmap,mincost);
                        }

                        temp=worklist[seqLen-1];
                        worklist[seqLen-1]=worklist[seqLen-2];
                        worklist[seqLen-2]=temp;
                        m++;
                        SubAlloc(worklist,mapArray,hadamard,minhadamard,minmap,mincost);

                        for(n=0; n<seqLen-1; n++)
                        {
                            temp=worklist[n];
                            worklist[n]=worklist[n+1];
                            worklist[n+1]=temp;
                            m++;
                            SubAlloc(worklist,mapArray,hadamard,minhadamard,minmap,mincost);
                        }

                        temp=worklist[0];
                        worklist[0]=worklist[1];
                        worklist[1]=temp;
                        m++;
                        SubAlloc(worklist,mapArray,hadamard,minhadamard,minmap,mincost);
                    }
                }

                mapArray=minmap;
                hadamard=minhadamard;
                totalcost=totalcost+mincost;
                worklist.clear();
            }

            for(j=0;j<qubitNum;j++)
                vacant[j]=true;

            i=record-1;
            record=seq.size();
            cnt=0;
            flag=false;
        }
    }

    return totalcost;
}


void HardwareB::SubAlloc(vector<vector<int>> worklist,vector<int> mapArray,vector<bool> hadamard,vector<bool>& minhadamard,vector<int>& minmap,int& mincost)
{
    unsigned int i;
    int j,current,next,dest,temp,cost=0;

    for(i=0; i<worklist.size(); i++)
    {
        for(j=0; j<qubitNum; j++)
        {
            if(mapArray[j]==worklist[i][0])
                current=j;

            if(mapArray[j]==worklist[i][1])
                dest=j;
        }

        next=routeMatrix[current][dest];

        if(next==dest)
        {
            if(archMatrix[current][next])
            {
                cost++;
                hadamard[current]=false;
                hadamard[next]=false;
            }

            else
            {
                cost=cost+5;

                if(hadamard[current])
                    cost=cost-2;
                else
                    hadamard[current]=true;

                if(hadamard[next])
                    cost=cost-2;
                else
                    hadamard[next]=true;
            }
        }

        else
        {
            while(routeMatrix[next][dest]!=dest)
            {
                temp=mapArray[current];
                mapArray[current]=mapArray[next];
                mapArray[next]=temp;

                cost=cost+7;

                hadamard[current]=false;
                hadamard[next]=false;

                current=next;
                next=routeMatrix[current][dest];
            }

            if(archMatrix[current][next] && archMatrix[next][dest])
            {
                cost=cost+4;

                hadamard[current]=false;
                hadamard[next]=false;
                hadamard[dest]=false;
            }

            else if(!archMatrix[current][next] && !archMatrix[next][dest])
            {
                cost=cost+10;

                if(hadamard[current])
                    cost=cost-2;
                else
                    hadamard[current]=true;

                if(hadamard[next])
                    cost=cost-2;
                else
                    hadamard[next]=true;

                if(hadamard[dest])
                    cost=cost-2;
                else
                    hadamard[dest]=true;
            }

            else if(archMatrix[current][next] && !archMatrix[next][dest])
            {
                cost=cost+10;

                hadamard[current]=false;

                if(hadamard[next])
                {
                    cost=cost-2;
                    hadamard[next]=false;
                }

                if(hadamard[dest])
                    cost=cost-2;
                else
                    hadamard[dest]=true;
            }

            else
            {
                cost=cost+10;

                if(hadamard[current])
                    cost=cost-2;
                else
                    hadamard[current]=true;

                hadamard[next]=true;

                hadamard[dest]=false;
            }
        }

    }

    if(cost<mincost)
    {
        mincost=cost;
        minmap=mapArray;
        minhadamard=hadamard;
    }
}

///////////////  Ending: Hardware with bridge  /////////////////////


void RandSeqGen(vector<vector<int>> &seq,int qubitNum,int seqLen);

int GetSeq(vector<vector<int>> &seq,string fname);

void PrintSeq(vector<vector<int>> seq);

int GetSeqList(vector<string> &fileList, string directory);

int main()
{
    int i,j,k,cntA,cntB,cost,fcount;
    int changetime=10000;

    HardwareB origin("ibmqxm");
    HardwareB arch("ibmqxm");

    arch.PrintArchMatrix();

    vector<string> fileList;
    vector<vector<int>> seq;
    vector<int> result;

    string directory="/home/tilmto/CodeBlocks/Architecture/seq";
    fcount=GetSeqList(fileList,directory);

    ofstream os1("better_arch",ios::out);

    ofstream os2("worse_arch",ios::out);

    if(!os1 || !os2)
    {
        cout << "Cannot write better_arch / worse_arch." << endl;
        exit(1);
    }

    for(i=0; i<fcount; i++)
    {
        GetSeq(seq,"seq/"+fileList[i]);

        arch.InitMap(seq);

        cost=arch.Alloc(seq);

        result.push_back(cost);
    }

    for(k=1; k<=changetime; k++)
    {
        cout << "Try the " << k << "th arch" << endl;

        if(k%50)
            arch=origin;

        arch.changeArch();

        cntA=0;
        cntB=0;

        for(i=0; i<fcount; i++)
        {
            GetSeq(seq,"seq/"+fileList[i]);

            arch.InitMap(seq);

            cost=arch.Alloc(seq);

            if(cost<result[i])
                cntB++;
            
            else if(result[i]<cost)
                cntA++;
        }

        cout << "new one: " << cntB << " ibmqxm: " << cntA << endl;

        if(cntB>cntA)
        {
            cout << "The better architecture is" << endl;
            arch.PrintArchMatrix();

            os1 << "new one: " << cntB << " ibmqxm: " << cntA << endl;

            for(i=0; i<arch.qubitNum; i++)
                for(j=0; j<arch.qubitNum; j++)
                {
                    os1 << arch.archMatrix[i][j] << " ";
                    if(j==arch.qubitNum-1)
                        os1 << endl;
                }

            os1 << endl;
        }

        else
        {
            cout << "Worse than ibmqxm" << endl;
            cout << endl;

            os2 << "new one: " << cntB << " ibmqxm: " << cntA << endl;

            for(i=0; i<arch.qubitNum; i++)
                for(j=0; j<arch.qubitNum; j++)
                {
                    os2 << arch.archMatrix[i][j] << " ";
                    if(j==arch.qubitNum-1)
                        os2 << endl;
                }

            os2 << endl;
        }
    }

    os1.close();

    os2.close();

    return 0;
}


void RandSeqGen(vector<vector<int>> &seq,int qubitNum,int seqLen)
{
    int cqubit,squbit;
    int i=0;
    srand((int)time(0));

    while(i<seqLen)
    {
        cqubit=rand()%qubitNum;
        squbit=rand()%qubitNum;
        if(cqubit!=squbit)
        {
            seq.push_back(vector<int>(2));
            seq[i][0]=cqubit;
            seq[i][1]=squbit;
            i++;
        }
    }
}


int GetSeq(vector<vector<int>> &seq,string fname)
{
    int i=0;
    int first,second,scount=0;

    seq.clear();

    ifstream is(fname,ios::in);

    if(!is)
    {
        cout << "No such seq file." << endl;
        exit(1);
    }

    while(!is.eof())
    {
        is >> first;
        is >> second;

        if(first<0)
            scount++;

        seq.push_back(vector<int>(2));
        seq[i][0]=first;
        seq[i][1]=second;
        i++;
    }

    seq.pop_back();

    is.close();

    return scount;
}


void PrintSeq(vector<vector<int>> seq)
{
    cout << "Dependency Sequence:"<< endl;

    for(unsigned int i=0; i<seq.size(); i++)
        cout << "( " << seq[i][0] << " , " << seq[i][1] << " )" <<endl;
}


int GetSeqList(vector<string> &fileList, string directory)
{
    directory = directory.append("/");

    DIR *p_dir;
    const char* str = directory.c_str();

    p_dir = opendir(str);
    if( p_dir == NULL)
    {
        cout<< "can't open :" << directory << endl;
    }

    struct dirent *p_dirent;

    while ( p_dirent = readdir(p_dir))
    {
        string tmpFileName = p_dirent->d_name;
        if( tmpFileName == "." || tmpFileName == "..")
        {
            continue;
        }
        else
        {
            fileList.push_back(tmpFileName);
        }
    }
    closedir(p_dir);

    return fileList.size();
}


int frac(int n)
{
    if(n==0 || n==1)
        return 1;
    else
        return n*frac(n-1);
}




