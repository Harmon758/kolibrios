#include "sst.h"
#include <string.h>
#ifndef KOS32
#include <time.h>
#else
#include <kolibrisys.h>
#endif

void dstrct() {
	/* Finish with a BANG! */
	chew();
	if (damage[DCOMPTR] != 0.0) {
		/* prout("Computer damaged; cannot execute destruct sequence."); */
		prout("�������� ���०���; ���������� �������� ᠬ�㭨�⮦����.");
		return;
	}
	skip(1);
/* 	prouts("---WORKING---"); skip(1);
	prout("SELF-DESTRUCT-SEQUENCE-ACTIVATED");
 */	prouts("---��������---"); skip(1);
	prout("���������-���������������-������������");
	prouts("   10"); skip(1);
	prouts("       9"); skip(1);
	prouts("          8"); skip(1);
	prouts("             7"); skip(1);
	prouts("                6"); skip(1);
/* 	prout("ENTER-CORRECT-PASSWORD-TO-CONTINUE-");
	prout("SELF-DESTRUCT-SEQUENCE-OTHERWISE-");
	prout("SELF-DESTRUCT-SEQUENCE-WILL-BE-ABORTED");
 */	prout("�������-���������-������-���-�����������-");
	prout("�������-���������-���������������-�����-");
	prout("���������-���������������-�����-������������");
	scan();
	chew();
	if (strcmp(passwd, citem) != 0) {
/* 		prouts("PASSWORD-REJECTED;"); skip(1);
		prout("CONTINUITY-EFFECTED");
 */		prouts("������-�������;"); skip(1);
		prout("�������������-����������");
		skip(1);
		return;
	}
	prouts("������-������"); skip(1); /*PASSWORD-ACCEPTED*/
	prouts("                   5"); skip(1);
	prouts("                      4"); skip(1);
	prouts("                         3"); skip(1);
	prouts("                            2"); skip(1);
	prouts("                              1"); skip(1);
	if (Rand() < 0.15) {
		prouts("������-��������-���"); /*GOODBYE-CRUEL-WORLD*/
		skip(1);
	}
	skip(2);
	kaboom();
}

void kaboom(void) {
	stars();
	if (ship==IHE) prouts("***");
	prouts("********* ���ய�� ");/*Entropy of*/
	crmshp();
	prouts(" ���ᨬ���஢��� *********");/*maximized*/
	skip(1);
	stars();
	skip(1);
	if (nenhere != 0) {
		double whammo = 25.0 * energy;
		int l=1;
		while (l <= nenhere) {
			if (kpower[l]*kdist[l] <= whammo) 
				deadkl(kx[l],ky[l], quad[kx[l]][ky[l]], kx[l], ky[l]);
			l++;
		}
	}
	finish(FDILITHIUM);
}
				

void finish(FINTYPE ifin) {
	int igotit = 0;
	alldone = 1;
	skip(3);
	/* printf("It is stardate %.1f .\n\n", d.date); */
	printf("�������� �६� %.1f .\n\n", d.date);
	switch (ifin) {
		case FWON: // Game has been won
			if (d.nromrem != 0)
				/* printf("The remaining %d Romulans surrender to Starfleet Command.\n", */
				printf("��᫥���� %d ஬㫠�楢 ᤠ���� �� ������� ��������� ������������.\n",
					   d.nromrem);


/*             prout("You have smashed the Klingon invasion fleet and saved");
            prout("the Federation.");
 */            prout("�� ᫮���� �ॡ�� ��থ��� ��������� � ᯠ᫨ �������.");

#ifdef CAPTURE
            if (alive && brigcapacity-brigfree > 0) { // captured Klingon crew will get transfered to starbase
                kcaptured += brigcapacity-brigfree;
                /* printf("The %d captured Klingons are transferred to Star Fleet Command.\n", */
                printf("��墠祭�� � ���� %d ��������� ��।��� ��������� ������������.\n",
                       brigcapacity-brigfree);
            }
#endif
			gamewon=1;
			if (alive) {
                double badpt;

				badpt = 5.*d.starkl + casual + 10.*d.nplankl +
						45.*nhelp+100.*d.basekl;
				if (ship == IHF) badpt += 100.0;
				else if (ship == 0) badpt += 200.0;
				if (badpt < 100.0) badpt = 0.0;	// Close enough!
				if (d.date-indate < 5.0 ||
					// killsPerDate >= RateMax
					(d.killk+d.killc+d.nsckill)/(d.date-indate) >=
					0.1*skill*(skill+1.0) + 0.1 + 0.008*badpt) {
					skip(1);
					/* prout("In fact, you have done so well that Starfleet Command"); */
					prout("�� ������ �ࠢ����� � ��������, � �������� ������������");
					switch (skill) {
						case SNOVICE:
							/* prout("promotes you one step in rank from \"Novice\" to \"Fair\"."); */
							prout("����蠥� ��� � ������ � \"����窠\" �� \"������\".");
							break;
						case SFAIR:
							/* prout("promotes you one step in rank from \"Fair\" to \"Good\"."); */
							prout("����蠥� ��� � ������ � \"�����������饣�\" �� \"�⫨筨��\".");
							break;
						case SGOOD:
							/* prout("promotes you one step in rank from \"Good\" to \"Expert\"."); */
							prout("����蠥� ��� � ������ � \"�⫨筨��\" �� \"��ᯥ��\".");
							break;
						case SEXPERT:
							/* prout("promotes you to Commodore Emeritus."); */
							prout("����蠥� ��� � ������ �� ��������� �������.");
							skip(1);
/* 							prout("Now that you think you're really good, try playing");
							prout("the \"Emeritus\" game. It will splatter your ego.");
 */							prout("������, �᫨ �� ��⠥� ᥡ� ॠ�쭮 �����, ���஡��");
							prout("�஢��� ᫮����� \"�������\". �� ��宬��� ��� �.");
							break;
						case SEMERITUS:
							skip(1);
/* 							prout("Computer-  ERROR-ERROR-ERROR-ERROR");
							skip(1);
							prout("  YOUR-SKILL-HAS-EXCEEDED-THE-CAPACITY-OF-THIS-PROGRAM");
							prout("  THIS-PROGRAM-MUST-SURVIVE");
							prout("  THIS-PROGRAM-MUST-SURVIVE");
							prout("  THIS-PROGRAM-MUST-SURVIVE");
							prout("  THIS-PROGRAM-MUST?- MUST ? - SUR? ? -?  VI");
							skip(1);
							prout("Now you can retire and write your own Star Trek game!");
 */							prout("��������-  ������-������-������-������");
							skip(1);
							prout("  ���-����-���������-�����������-����-���������");
							prout("  ���������-������-������");
							prout("  ���������-������-������");
							prout("  ���������-������-������");
							prout("  ���������-������-?- ������ ? - ���? ? -?  ���");
							skip(1);
							prout("������ ����� � ᯮ������ ��让 �� � ��ᠢ�� � ������� ᮡ�⢥���� ���� ��� �४!");
							skip(1);
							break;
					}
					if (skill > SGOOD) {
						if (thawed
#ifdef DEBUG
							&& !idebug
#endif
							)
							/* prout("You cannot get a citation, so..."); */
							prout("��� �� ������� � ������, ⠪ ��...");
						else {
/* 							prout("Do you want your Commodore Emeritus Citation printed?");
							proutn("(You need a 132 column printer.)");
 */							prout("�� ��� �������� ᯨ᮪ ᢮�� ��᪠�뢠���, �������� �������?");
							proutn("(��� ���ॡ���� 132-ᨬ����� �ਭ��.)");
							chew();
							if (ja()) {
								igotit = 1;
							}
						}
					}
				}
				// Only grant long life if alive (original didn't!)
				skip(1);
				/* prout("LIVE LONG AND PROSPER."); */
				prout("������ ��� ����� � �����������.");
			}
			score(0);
			if (igotit != 0) plaque();
			return;
		case FDEPLETE: // Federation Resources Depleted
/* 			prout("Your time has run out and the Federation has been");
			prout("conquered.  Your starship is now Klingon property,");
			prout("and you are put on trial as a war criminal.  On the");
			proutn("basis of your record, you are ");
 */			prout("��� �६� ��諮 � ������� �뫠 ��墠祭�.");
			prout("��� ��ࠡ�� ⥯��� ��䥩 ���������,");
			prout("� ��� ���� �㤨�� ��� �������� ����㯭���.");
			proutn("���ᬮ�ॢ ��� ����⢨�, �� ");
			if (d.remkl*3.0 > inkling) {
				prout("��ࠢ����.");/*aquitted*/
				skip(1);
				prout("������ ��� ����� � �����������.");/*LIVE LONG AND PROSPER*/
			}
			else {
/* 				prout("found guilty and");
				prout("sentenced to death by slow torture.");
 */				prout("�ਧ���� �������");
				prout("� �ਣ���७� � ᬥ�� ��� ��⪠��.");
				alive = 0;
			}
			score(0);
			return;
		case FLIFESUP:
/* 			prout("Your life support reserves have run out, and");
			prout("you die of thirst, starvation, and asphyxiation.");
			prout("Your starship is a derelict in space.");
 */			prout("� ��� ���稫��� ������ ������ � ��� �");
			prout("�� � �������� 㬥૨ �� ������ � �����, ��������.");
			prout("��� ����� ��ࠡ�� �३��� � ��ᬮ�.");
			break;
		case FNRG:
/* 			prout("Your energy supply is exhausted.");
			skip(1);
			prout("Your starship is a derelict in space.");
 */			prout("� ��ࠡ�� ���稫��� ������ �ࣨ�.");
			skip(1);
			prout("��� ����� ��ࠡ�� �३��� � ��ᬮ�.");
			break;
		case FBATTLE:
/* 			proutn("The ");
			crmshp();
			prout("has been destroyed in battle.");
			skip(1);
			prout("Dulce et decorum est pro patria mori.");
 */			proutn("��� ��ࠡ�� ");
			crmshp();
			prout("�� 㭨�⮦�� � ���.");
			skip(1);
			prout("Dulce et decorum est pro patria mori.");
			prout("/������ � �४�᭠ �� த��� ᬥ���./");
			break;
		case FNEG3:
/* 			prout("You have made three attempts to cross the negative energy");
			prout("barrier which surrounds the galaxy.");
			skip(1);
			prout("Your navigation is abominable.");
 */			prout("�� ᤥ���� �� ����⪨ ������ ����� ����⥫쭮� �ࣨ�");
			prout(" ���㦠�騩 �����⨪�.");
			skip(1);
			prout("�� 㦠�� ��������.");
			score(0);
			return;
		case FNOVA:
/* 			prout("Your starship has been destroyed by a nova.");
			prout("That was a great shot.");
 */			prout("��� ��ࠡ�� �� 㭨�⮦�� ����� �������.");
			prout("����� ����⪠.");
			skip(1);
			break;
		case FSNOVAED:
/* 			proutn("The ");
			crmshp();
			prout(" has been fried by a supernova.");
			prout("...Not even cinders remain...");
 */			proutn("��� ��ࠡ�� ");
			crmshp();
			prout(" �� 㭨�⮦�� ᢥ�孮���.");
			prout("...� �� ��⠫��� ���� �����...");
			break;
		case FABANDN:
/* 			prout("You have been captured by the Klingons. If you still");
			prout("had a starbase to be returned to, you would have been");
			prout("repatriated and given another chance. Since you have");
			prout("no starbases, you will be mercilessly tortured to death.");
 */			prout("�� �뫨 ��墠祭� ����������. �᫨ �� � ��� ");
			prout("��⠫��� ��� ���� �⠭�� ��� �����饭��, �� ����� ��");
			prout("������� ��ன 蠭�. �� ��᪮��� �⠭権 ����� ���,");
			prout("��� ��ᯮ頤�� ������� �� ᬥ��.");
			break;
		case FDILITHIUM:
			/* prout("Your starship is now an expanding cloud of subatomic particles"); */
			prout("��� ��ࠡ�� �ॢ�⨫�� � ������ �㡠⮬��� ����");
			break;
		case FMATERIALIZE:
/* 			prout("Starbase was unable to re-materialize your starship.");
			prout("Sic transit gloria muntdi");
 */			prout("�⠭�� �� ᬮ��� ���⭮ ���ਠ�������� ��� ��ࠡ��.");
			prout("Sic transit gloria muntdi");
			prout("/��� ��室�� ���᪠� ᫠��/");
			break;
		case FPHASER:
/* 			proutn("The ");
			crmshp();
			prout(" has been cremated by its own phasers.");
 */			proutn("��� ��ࠡ�� ");
			crmshp();
			prout(" ᠬ�㭨�⮦���� ����� ᮡ�⢥���� 䠧�஢.");
			break;
		case FLOST:
/* 			prout("You and your landing party have been");
			prout("converted to energy, dissipating through space.");
 */			prout("�� � ��㯯� ��ᠤ�� ");
			prout("�ॢ�⨫��� � ����� �ࣨ�, ��뫥���� � ��ᬮ�.");
			break;
		case FMINING:
/* 			prout("You are left with your landing party on");
			prout("a wild jungle planet inhabited by primitive cannibals.");
			skip(1);
			prout("They are very fond of \"Captain Kirk\" soup.");
			skip(1);
			proutn("Without your leadership, the ");
			crmshp();
			prout(" is destroyed.");
 */			prout("�� ��⠫��� ����� � ��㯯�� ��ᠤ��");
			prout("�� ����� ������, ��ᥫ����� �������᪨�� �����������.");
			skip(1);
			prout("��� �祭� ���� �� \"����⠭ ���\".");
			skip(1);
			proutn("��� ��襣� ������������, ��ࠡ�� ");
			crmshp();
			prout(" �� 㭨�⮦��.");
			break;
		case FDPLANET:
/* 			prout("You and your mining party perish.");
			skip(1);
			prout("That was a great shot.");
 */			prout("�� ���᫥��� �ய��� ����� � ���ࠬ�.");
			skip(1);
			prout("�� �뫠 ���� ����⪠.");
			skip(1);
			break;
		case FSSC:
			/* prout("The Galileo is instantly annihilated by the supernova."); */
			prout("������� �� ��������� ��������஢�� �㯥୮���.");
			// no break;
		case FPNOVA:
/* 			prout("You and your mining party are atomized.");
			skip(1);
			proutn("Mr. Spock takes command of the ");
			crmshp();
			prout(" and");
			prout("joins the Romulans, reigning terror on the Federation.");
 */			prout("�� � ��㯯�� ���஢ �ᯠ���� �� �⮬�.");
			skip(1);
			proutn("����� ���� �ਭ� ������������ ");
			crmshp();
			prout(" �");
			prout("��ᮥ������� � ஬㫠�栬, �� ࠧ��襭�� � ���� �� �ᥩ �����樨.");
			break;
		case FSTRACTOR:
/* 			prout("The shuttle craft Galileo is also caught,");
			prout("and breaks up under the strain.");
			skip(1);
			prout("Your debris is scattered for millions of miles.");
			proutn("Without your leadership, the ");
			crmshp();
			prout(" is destroyed.");
 */			prout("���� ������� ����� � ���� ����⢨� ᨫ����� ���,");
			prout("� �� ࠧ�ࢠ� ᨫ��� �⥭��.");
			skip(1);
			prout("��� ������� ����� �� �������� ����.");
			proutn("��� ��襣� ������������, ��ࠡ�� ");
			crmshp();
			prout(" �� 㭨�⮦��.");
			break;
		case FDRAY:
/* 			prout("The mutants attack and kill Spock.");
			prout("Your ship is captured by Klingons, and");
			prout("your crew is put on display in a Klingon zoo.");
 */			prout("��⠭�� �⠪����� � 㡨�� �����.");
			prout("��� ��ࠡ�� �� ��墠祭 ���������� � ⥯���");
			prout("��� ������� �।�⠢���� � �������᪮� �����થ.");
			break;
		case FTRIBBLE:
/* 			prout("Tribbles consume all remaining water,");
			prout("food, and oxygen on your ship.");
			skip(1);
			prout("You die of thirst, starvation, and asphyxiation.");
			prout("Your starship is a derelict in space.");
 */			prout("�ਡ��� ᮦࠫ� ��� ��⠢����� ����,");
			prout("��� � ��᫮த �� ��襬 ��ࠡ��.");
			skip(1);
			prout("�� � �������� 㬥૨ �� ������ � �����, ��������.");
			prout("��� ����� ��ࠡ�� �३��� � ��ᬮ�.");
			break;
		case FHOLE:
/* 			prout("Your ship is drawn to the center of the black hole.");
			prout("You are crushed into extremely dense matter.");
 */			prout("��� ����㫮 � 業�� �୮� ����.");
			prout("�� ⥯��� �᪫��⥫쭮 �����쪠� � ᢥ��⢥ठ� �뫨���.");
			break;
#ifdef CLOAKING
		case FCLOAK:
			ncviol++;
/* 			prout("You have violated the Treaty of Algeron.");
			prout("The Romulan Empire can never trust you again.");
 */			prout("�� ����訫� ����஭᪮� �����襭��.");
			prout("���㫠�᪠� ������ ������� ����� �� ������ ���.");
			break;
#endif
	}
#ifdef CLOAKING
	if (ifin!=FWON && ifin!=FCLOAK && iscloaked!=0) {
/* 		prout("Your ship was cloaked so your subspace radio did not receive anything.");
		prout("You may have missed some warning messages.");
 */		prout("��� ��ࠡ�� �� �������, ⠪ �� �������࠭ᢥ���� ࠤ�� �� ࠡ�⠫�.");
		prout("�� ����� �ய����� ������� �।�।�⥫�� ᮮ�饭��.");
		skip(1);
	}
#endif

	if (ship==IHF) ship= 0;
	else if (ship == IHE) ship = IHF;
	alive = 0;
	if (d.remkl != 0) {
		double goodies = d.remres/inresor;
		double baddies = (d.remkl + 2.0*d.remcom)/(inkling+2.0*incom);
		if (goodies/baddies >= 1.0+0.5*Rand()) {
/* 			prout("As a result of your actions, a treaty with the Klingon");
			prout("Empire has been signed. The terms of the treaty are");
 */			prout("� १���� ���� ���⢨�, �뫮 �����ᠭ� ��୮�");
			prout("ᮣ��襭�� � ����������. �᫮��� ᮣ��襭�� �뫨");
			if (goodies/baddies >= 3.0+Rand()) {
/* 				prout("favorable to the Federation.");
				skip(1);
				prout("Congratulations!");
 */				prout("�룮��묨 ��� �����樨.");
				skip(1);
				prout("����ࠢ�塞!");
			}
			else
				/* prout("highly unfavorable to the Federation."); */
				prout("�᪫��⥫쭮 ���룮��묨 ��� �����樨.");
		}
		else
			/* prout("The Federation will be destroyed."); */
			prout("������� �㤥� 㭨�⮦���.");
	}
	else {
/* 		prout("Since you took the last Klingon with you, you are a");
		prout("martyr and a hero. Someday maybe they'll erect a");
		prout("statue in your memory. Rest in peace, and try not");
		prout("to think about pigeons.");
 */		prout("�� ���ࠫ� ��᫥����� �������� � ᮡ�� �� �� ᢥ�.");
		prout("��� ��ந�᪠� ���⢠ �� �㤥� ����� �, ��������, ��⮬�� ����");
		prout("�������� ���� � ���� ����. �������� � ��஬ � ��ࠩ���");
		prout("�� �㬠�� � ������.");
		gamewon = 1;
	}
	score(0);
}

void score(int inGame) {
	double timused = d.date - indate;
    int ithperd, iwon, klship;
    int dnromrem = d.nromrem; // Leave global value alone

    if (!inGame) pause(0);

	iskill = skill;
	if ((timused == 0 || d.remkl != 0) && timused < 5.0) timused = 5.0;
	perdate = (d.killc + d.killk + d.nsckill)/timused;
	ithperd = 500*perdate + 0.5;
	iwon = 0;
	if (gamewon) iwon = 100*skill;
	if (ship == IHE) klship = 0;
	else if (ship == IHF) klship = 1;
	else klship = 2;
	if (gamewon == 0 || inGame) dnromrem = 0; // None captured if no win or if still in the game
	iscore = 10*d.killk + 50*d.killc + ithperd + iwon
			 - 100*d.basekl - 100*klship - 45*nhelp -5*d.starkl - casual
		 + 20*d.nromkl + 200*d.nsckill - 10*d.nplankl + dnromrem;
#ifdef CLOAKING
	iscore -= 100*ncviol;
#endif
#ifdef CAPTURE
	iscore += 3*kcaptured;
#endif
	if (alive == 0) iscore -= 200;
	skip(2);
/*     if (inGame) prout("Your score so far --");
    else prout("Your score --");
 */    if (inGame) prout("�� ����� ������ ��� ����� --");
    else prout("��� ����� --");
	if (d.nromkl)
	 /* printf(d.nromkl> 1 ? "%6d Romulan ships destroyed            %5d\n" : "%6d Romulan ship destroyed             %5d\n", */
		printf(d.nromkl> 1 ? "%6d ஬㫠�᪨� ��ࠡ��� 㭨�⮦���        %5d\n" : "%6d ஬㫠�᪨� ��ࠡ��� 㭨�⮦���             %5d\n",
			   d.nromkl, 20*d.nromkl);
	if (dnromrem)
	 /* printf(dnromrem > 1 ? "%6d Romulan ships captured             %5d\n" : "%6d Romulan ship captured              %5d\n", */
		printf(dnromrem > 1 ? "%6d ஬㫠�᪨� ��ࠡ��� ��墠祭�         %5d\n" : "%6d ஬㫠�᪨� ��ࠡ��� ��墠祭�              %5d\n",
			   dnromrem, dnromrem);
	if (d.killk)
	 /* printf(d.killk > 1 ? "%6d ordinary Klingon ships destroyed   %5d\n" : "%6d ordinary Klingon ship destroyed    %5d\n", */
		printf(d.killk > 1 ? "%6d ������ ��ࠡ��� ��������� 㭨�⮦���    %5d\n" : "%6d ������ ��ࠡ��� ��������� 㭨�⮦���    %5d\n",
			   d.killk,  10*d.killk);
	if (d.killc)
		/* printf(d.killc > 1 ? "%6d Klingon Commander ships destroyed  %5d\n" : "%6d Klingon Commander ship destroyed   %5d\n", */
		printf(d.killc > 1 ? "%6d �������᪨� �������஢ 㭨�⮦���       %5d\n" : "%6d �������᪨� �������஢ 㭨�⮦���   %5d\n",
			   d.killc, 50*d.killc);
	if (d.nsckill)
     /* printf("%6d Super-Commander ship destroyed     %5d\n", */
		printf("%6d �㯥���������஢ 㭨�⮦���        %5d\n",
			   d.nsckill, 200*d.nsckill);
	if (ithperd)
     /* printf("%6.2f Klingons per stardate              %5d\n", */
		printf("%6.2f ��������� � �।��� �� ����       %5d\n",
			   perdate, ithperd);
#ifdef CAPTURE
	if (kcaptured)
		/* printf(kcaptured > 1 ? "%6d Klingons captured                  %5d\n" : "%6d Klingon captured                   %5d\n", */
		printf(kcaptured > 1 ? "%6d ��������� ��墠祭�                  %5d\n" : "%6d ��������� ��墠祭�                   %5d\n",
		        kcaptured, 3*kcaptured);
#endif
	if (d.starkl)
		/* printf(d.starkl > 1 ? "%6d stars destroyed by your action     %5d\n" : "%6d star destroyed by your action      %5d\n", */
		printf(d.starkl > 1 ? "%6d ����� 㭨�⮦��� ��訬� ����⢨ﬨ     %5d\n" : "%6d ����� 㭨�⮦��� ��訬� ����⢨ﬨ      %5d\n",
			   d.starkl, -5*d.starkl);
	if (d.nplankl)
		/* printf(d.nplankl > 1 ? "%6d planets destroyed by your action   %5d\n" : "%6d planet destroyed by your action    %5d\n", */
		printf(d.nplankl > 1 ? "%6d ������ 㭨�⮦��� ��訬� ����⢨ﬨ    %5d\n" : "%6d ������ 㭨�⮦��� ��訬� ����⢨ﬨ    %5d\n",
			   d.nplankl, -10*d.nplankl);
	if (d.basekl)
		printf(d.basekl > 1 ? "%6d �⠭権 㭨�⮦��� ��訬� ����⢨ﬨ     %5d\n" : "%6d �⠭権 㭨�⮦��� ��訬� ����⢨ﬨ      %5d\n",
			   d.basekl, -100*d.basekl);
	if (nhelp)
		printf(nhelp > 1 ? "%6d �맮��� � ����� � �⠭権       %5d\n" : "%6d �맮��� � ����� � �⠭権        %5d\n",
			   nhelp, -45*nhelp);
	if (casual)
		printf(casual > 1 ? "%6d ����� �।� �����                %5d\n" : "%6d ����� �।� �����                  %5d\n",
			   casual, -casual);
	if (klship)
		printf(klship > 1 ? "%6d ��ࠡ��� ����ﭮ ��� 㭨�⮦���            %5d\n" : "%6d ��ࠡ��� ����ﭮ ��� 㭨�⮦���             %5d\n",
			   klship, -100*klship);
#ifdef CLOAKING
	if (ncviol>0)
		printf(ncviol > 1 ? "%6d ����襭�� ����஭᪮�� ᮣ��襭��       %5d\n" : "%6d ����襭�� ����஭᪮�� ᮣ��襭��        %5d\n",
		       ncviol, -100*ncviol);
#endif
	if (alive==0)
		/* prout("Penalty for getting yourself killed        -200"); */
		prout("���� �� �, �� �� ���� ᥡ� 㡨��        -200");
	if (gamewon) {
		skip(1);
		/* proutn("Bonus for winning "); */
		proutn("����� �� ������ ");
		switch (skill) {
/* 			case SNOVICE: proutn("Novice game  "); break;
			case SFAIR: proutn("Fair game    "); break;
			case SGOOD: proutn("Good game    "); break;
			case SEXPERT: proutn("Expert game  "); break;
			case SEMERITUS: proutn("Emeritus game"); break;
 */			case SNOVICE: proutn("��� ����窠  "); break;
			case SFAIR: proutn("��� ������    "); break;
			case SGOOD: proutn("��� �⫨筨��    "); break;
			case SEXPERT: proutn("��� ��ᯥ��  "); break;
			case SEMERITUS: proutn("��� �������"); break;
		}
		printf("           %5d\n", iwon);
	}
	skip(2);
    printf("�⮣��� ����                               %5d\n", iscore);/*TOTAL SCORE*/
    /* if (inGame && skill < SGOOD) printf("REMEMBER--The score doesn't really matter until the mission is accomplished!\n"); */
    if (inGame && skill < SGOOD) printf("�������--����� ��祣� �� �����, ���� ����� �� ����祭�!\n");
}

void plaque(void) {
	FILE *fp=NULL;
#ifndef KOS32	
	time_t t;
#else
	int kos_date, kos_time;
#endif
	char *timestring;
	int nskip;
	char winner[128];
	skip(2);
	
	while (fp == NULL) {
		/* printf("File or device name for your plaque:"); */
		printf("������ ��� 䠩�� ��� ���� ���⨦����:");
#ifndef KOS32		
		fgets(winner, 128, stdin);
#else
		gets(winner);
#endif		
		winner[strlen(winner)-1] = '\0';
		fp = fopen(winner, "w");
		if (fp==NULL) {
			printf("����୮� ���.\n");/*Invalid name*/
		}
	}

	/* printf("Enter name to go on plaque (up to 30 characters):"); */
	printf("������ ��� ��� ����⭮�� ���窠 (���ᨬ� 30 ᨬ�����):");
#ifndef KOS32		
		fgets(winner, 128, stdin);
#else
		gets(winner);
#endif		
	winner[strlen(winner)-1] = '\0';
	winner[30] = '\0';
	nskip = 64 - strlen(winner)/2;

	fprintf(fp,"\n\n\n\n");
	/* --------DRAW ENTERPRISE PICTURE. */
	fprintf(fp, "                                                                EEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE\n" );
	fprintf(fp, "                                      EEE                      E  : :                                         :  E\n" );
	fprintf(fp, "                                    EE   EEE                   E  : :                   NCC-1701              :  E\n");
	fprintf(fp, "                    EEEEEEEEEEEEEEEE        EEEEEEEEEEEEEEE    E  : :                                         : E\n");
	fprintf(fp, "                     E                                     E    EEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE\n");
	fprintf(fp, "                      EEEEEEEEE               EEEEEEEEEEEEE                 E  E\n");
	fprintf(fp, "                               EEEEEEE   EEEEE    E          E              E  E\n");
	fprintf(fp, "                                      EEE           E          E            E  E\n");
	fprintf(fp, "                                                       E         E          E  E\n");
	fprintf(fp, "                                                         EEEEEEEEEEEEE      E  E\n");
	fprintf(fp, "                                                      EEE :           EEEEEEE  EEEEEEEE\n");
	fprintf(fp, "                                                    :E    :                 EEEE       E\n");
	fprintf(fp, "                                                   .-E   -:-----                       E\n");
	fprintf(fp, "                                                    :E    :                            E\n");
	fprintf(fp, "                                                      EE  :                    EEEEEEEE\n");
	fprintf(fp, "                                                       EEEEEEEEEEEEEEEEEEEEEEE\n");
	fprintf(fp, "\n\n\n");
	fprintf(fp, "                                                       U. S. S. ENTERPRISE\n");
	fprintf(fp, "\n\n\n\n");
	/* fprintf(fp, "                                  For demonstrating outstanding ability as a starship captain\n"); */
	fprintf(fp, "                                  �� ������ �뤠�騥�� ���⨦���� � �������� ����⠭� ��ᬨ�᪮�� ��ࠡ��\n");
	fprintf(fp, "\n");
	/* fprintf(fp, "                                                Starfleet Command bestows to you\n"); */
	fprintf(fp, "                                                �������� ������������ �।�⠢��� ���\n");
	fprintf(fp, "\n");
	fprintf(fp,"%*s%s\n\n", nskip, "", winner);
/* 	fprintf(fp, "                                                           the rank of\n\n");
	fprintf(fp, "                                                       \"Commodore Emeritus\"\n\n");
 */	fprintf(fp, "                                                           � ������\n\n");
	fprintf(fp, "                                                       \"�������� �������\"\n\n");
	fprintf(fp, "                                                          ");
	switch (iskill) {
/* 		case SEXPERT: fprintf(fp," Expert level\n\n"); break;
		case SEMERITUS: fprintf(fp,"Emeritus level\n\n"); break;
		default: fprintf(fp," Cheat level\n\n"); break;
 */		case SEXPERT: fprintf(fp," �஢��� ��ᯥ��\n\n"); break;
		case SEMERITUS: fprintf(fp,"�஢��� �������\n\n"); break;
		default: fprintf(fp," ����� ���᪨� �஢���\n\n"); break;
	}
#ifndef KOS32	
	t = time(NULL);
	timestring = ctime(&t);
	/* fprintf(fp, "                                                 This day of %.6s %.4s, %.8s\n\n", */
	fprintf(fp, "                                                 ��� ���� %.6s %.4s, %.8s\n\n",
			timestring+4, timestring+20, timestring+11);
#else
	kos_date = _ksys_get_date();
	kos_time = _ksys_get_system_clock();
	/* fprintf(fp, "                                                 This day of %02i/%02i/%02i %02i:%02i:%02i\n\n", */
	fprintf(fp, "                                                 ��� ���� %02i/%02i/%02i %02i:%02i:%02i\n\n",
			kos_date >> 16, (kos_date & 0xFF00) >> 8, (kos_date & 0xFF) + 2000,
			kos_time & 0xFF, (kos_time & 0xFF00) >> 8, kos_time >> 16 );
#endif
/* 	fprintf(fp,"                                                        Your score:  %d\n\n", iscore);
	fprintf(fp,"                                                    Klingons per stardate:  %.2f\n", perdate);
 */	fprintf(fp,"                                                        ��� ����:  %d\n\n", iscore);
	fprintf(fp,"                                                    ��������� �� ����:  %.2f\n", perdate);
	fclose(fp);
}
