#include <iostream>
#include<fstream>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <vector>
#include <time.h>
#include <sstream>
#include <algorithm>
#include <iomanip>
using namespace std;
const int NUM = 3; //Define the number of clusters
struct Vect
{
	string name;
	double LPM;
	double PVM;
	double PEM;
	double PTM;
	double PBM;
};
struct Dis
{
    string name;
    double d;
};
struct MDis
{
    Dis dis[2000];
    int posK;      // The closest cluster
};
struct Program{
    string name;
    string id;
    string PGName;
    double PBM;
    int cnt;
};
int cmp1(Dis a,Dis b)
{
    return a.d<b.d;
}
int cmp2(Program a,Program b)
{
    return a.id<b.id;
}
int cmp3(Program a,Program b)
{
    return a.cnt>b.cnt;
}
void parseInit(Vect *s, string str) {
	istringstream istr(str);
	istr >> s->name;
	istr >> s->LPM;
	istr >> s->PVM;
	istr >> s->PEM;
	istr >> s->PTM;
    istr >> s->PBM;
	istr.clear();
};
void parseInit2(Program *p,string str) {
    istringstream istr(str);
    istr >> p->name;
    istr >> p->id;
    istr >> p->PGName;
    istr >> p->PBM;
    istr.clear();
}
double getDist(Vect A, Vect B)  //Calculate the square of distance
{
	double sim;
	sim = sqrt((A.LPM - B.LPM) * (A.LPM - B.LPM) + (A.PVM - B.PVM) * (A.PVM - B.PVM) + (A.PEM - B.PEM) * (A.PEM - B.PEM) + (A.PTM - B.PTM) * (A.PTM - B.PTM) + (A.PBM - B.PBM) * (A.PBM - B.PBM));
	return sim;
}

void getMinDis(vector<Vect> test,MDis Mindis[],Vect means[],vector<Vect> *ans) 
 //Calculate the shortest distance between each investor and the cluster
{
    int k;
    for (int i=0;i<test.size();i++)
    {
        double t1=getDist(test[i],means[0]);
        Mindis[i].posK=0;
        for (int j=1;j<NUM;j++)
            if (t1>getDist(test[i],means[j]))
        {
            t1=getDist(test[i],means[j]);
            Mindis[i].posK=j;
        }
        k=Mindis[i].posK;
        for (int p=0;p<ans[k].size();p++)
        {
            Mindis[i].dis[p].d=getDist(ans[k][p],test[i]);
            Mindis[i].dis[p].name=ans[k][p].name;
        }
    }
}

// Calculate the center of each cluster by average distance
Vect getMeansC(vector<Vect> t)
{
	int num = t.size();
	double meansL = 0;
	double meansV = 0;
	double meansE = 0;
	double meansT = 0;
	double meansB = 0;
	for (int i = 0; i<num; i++)
	{
		meansL += t[i].LPM;
		meansV += t[i].PVM;
		meansE += t[i].PEM;
		meansT += t[i].PTM;
		meansB += t[i].PBM;
	}
	Vect c;
	c.LPM = meansL / num;
	c.PVM = meansV / num;
	c.PEM = meansE / num;
	c.PTM = meansT / num;
	c.PBM = meansB / num;
	return c;
}

// Get the Criterion function value. The algorithm will stop when the Criterion function value converges

double getE(vector<Vect> classes[], Vect means[])
{
	double sum = 0;
	for (int i = 0; i<NUM; i++)
	{
		vector<Vect> v = classes[i];
		for (int j = 0; j<v.size(); j++)
			sum += getDist(v[j], means[i]);
	}
	return sum;
}

int searchMinC(Vect t, Vect means[], int n, double *D)
{
	int c = 0;
	double d = getDist(t, means[0]);
	for (int i = 1; i<n; i++)
	{
		double tmp = getDist(t, means[i]);
		if (tmp < d)
		{
			c = i;
			d = tmp;
		}
	}
	if (D) *D = d;
	return c;
}
void Find(vector<Vect> init, Vect means[])
{
	srand(time(NULL));
	double *D = new double[init.size()];
	means[0] = init[rand() % init.size()];
	for (int i = 1; i < NUM; i++)
	{
		double sum = 0;
		for (int j = 0; j < init.size(); j++)
		{
			searchMinC(init[j], means, i, D + j);
			sum += D[j];
		}
		sum = rand() % (int)sum;
		for (int j = 0; j < init.size(); j++)
		{
			if ((sum -= D[j]) > 0) continue;
			means[i] = init[j];
			break;
		}
	}
	delete[] D;
}
void kMeans(vector<Vect> init, vector<Vect> *ans,Vect *means)
{
	vector<Vect> classes[NUM];
	double newE, oldE = -1;
	Find(init, means);
	newE = getE(classes, means); 
	for (int i = 0; i<NUM; i++)
		classes[i].clear();
	while (fabs(newE - oldE) >= 1)
	{
		for (int i = 0; i<init.size(); i++)
		{
			int toC = searchMinC(init[i], means, NUM, NULL);
			classes[toC].push_back(init[i]);
		}
		for (int i = 0; i<NUM; i++)
			ans[i] = classes[i];
		for (int i = 0; i<NUM; i++)
			means[i] = getMeansC(classes[i]);
		oldE = newE;
		newE = getE(classes, means);
		for (int i = 0; i<NUM; i++)
			classes[i].clear();
	}


	for (int i = 0; i<NUM; i++)
	{
		cout << "Class" << i + 1 << ":" << endl;
		for (int j = 0; j<ans[i].size(); j++)
			cout << ans[i][j].name<< setw(8) << ans[i][j].LPM << setw(8) << ans[i][j].PVM << setw(8) << ans[i][j].PEM << setw(8) << ans[i][j].PTM << setw(8) << ans[i][j].PBM << endl;
		cout << endl;
	}
	// output the coordinate of every cluster's center
	for (int i = 0; i<NUM; i++)
		cout << "means[" << i + 1 << "]: " << means[i].LPM << " " << means[i].PVM << " " << means[i].PEM << " " << means[i].PTM << " " << means[i].PBM << endl;
}
int main()
{

	string s;
	MDis MinDis[53];
	vector<Vect> test(53);     
	vector<Vect> init(366);    
	Program pg[3170],t[1029];       
    int m,Nu,j,cntp,cntt,cntpt;
    j=0;
	fstream file("train.txt", ios::in);
	if (!file) {
		cout << "Error in file opening! File name:" << "train.txt" << endl;
		return 0;
	}
	while (getline(file, s)) {
		Vect *ve = new Vect;
		parseInit(ve, s);
		init[j] = *ve;
		if (init[j].PBM==NULL)
            init[j].PBM=0.00;
		j++;
	}
	cntt=j;
	file.close();

    j=0;
	fstream f2("test.txt",ios::in);
    if (!f2) {
		cout << "Error in file opening! File name:" << "test.txt" << endl;
		return 0;
	}
	while (getline(f2, s)) {
		Vect *ve = new Vect;
		parseInit(ve, s);
		test[j] = *ve;
		j++;
	}
	cntt=j;
	f2.close();
    /*for (int i=0;i<test.size();i++)
    {
        cout<<test[i].name<<setw(5)<<test[i].LPM<<setw(5)<<test[i].PBM<<endl;
    }*/

    j=0;
	fstream f3("train_investment_projects.txt", ios::in);
	if (!f3) {
		cout << "Error in file opening! File name:" << "train_investment_projects.txt" << endl;
		return 0;
	}
	while (getline(f3, s)) {
		Program *p = new Program;
		parseInit2(p, s);
		pg[j] = *p;
		j++;
	}
	cntp=j;
	f3.close();
    /*for (int i=0;i<pg.size();i++)
        cout<<pg[i].name<<" "<<pg[i].id<<endl;*/
    j=0;
    fstream f4("test_investment_projects.txt", ios::in);
	if (!f4) {
		cout << "Error in file opening! File name:" << "test_investment_projects.txt" << endl;
		return 0;
	}
	while (getline(f4, s)) {
		Program *p = new Program;
		parseInit2(p, s);
		t[j] = *p;
		j++;
	}
	cntpt=j;
	f4.close();


	vector<Vect> ans[NUM];
	Vect means[NUM];
	kMeans(init,ans,means);
    getMinDis(test,MinDis,means,ans);
    for (int i=0;i<cntt;i++)
       sort(MinDis[i].dis,MinDis[i].dis+cntt,cmp1);
    sort(pg,pg+cntp,cmp2);          
    sort(t,t+cntpt,cmp2);
    int kt=0;                       
    for (int p=1;p<cntpt;p++)      
        if (t[kt].id!=t[p].id)
        t[++kt].id=t[p].id;
    //cout<<"\n"<<kt<<endl;       
    for (int i=0;i<cntt;i++)
    {
        printf("Please input the information of %d investor in test data set:\n",i+1);
        cout<<"Input Nu to find the according investor in train set£º";
	    scanf("%d",&Nu);
         int k=0;                        //k is the number of projects
       pg[k].cnt=0;
         for (int j=0;j<Nu;j++)
        if (pg[0].name==MinDis[i].dis[j].name)
        {
            pg[0].cnt++;
            break;
        }
    for (int p=1;p<cntp;p++)         
    {
         if (pg[p].id!=pg[k].id)
         {
             pg[++k].id=pg[p].id;
             pg[k].cnt=0;
         }
        for (int j=0;j<Nu;j++)
            if (pg[p].name==MinDis[i].dis[j].name)
            {
                pg[k].cnt++;
                break;
            }
    }
    sort(pg,pg+cntp,cmp3);
    cout<<"Input m to find the most invested projects among Nu investors£º";
	scanf("%d",&m);
    cout<<"The id of m most invested projects among Nu investors are£º\n";
    for (int i=0;i<m;i++)
        cout<<pg[i].id<<endl;
    double c=0;
    for (int p=0;p<kt;p++)
    {
        for (int q=0;q<m;q++)
            if (t[p].id==pg[q].id)
          {
            c++;
            break;
          }
    }
       double ans=c/double(m);
       cout<<"The similarity is:"<<ans<<endl;
    }

	system("pause");
	return 0;
}
