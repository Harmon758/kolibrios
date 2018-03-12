#include "sst.h"

static char classes[4][2]={"","M","N","O"};
static int height;

static int consumeTime(void) {
/* I think most of this avoidance was caused by overlay scheme.
   Let's see what happens if all events can occur here */

//	double asave;
	ididit = 1;
#if 0
	/* Don't wory about this */
	if (future[FTBEAM] <= d.date+Time && d.remcom != 0 && condit != IHDOCKED) {
		/* We are about to be tractor beamed -- operation fails */
		return 1;
	}
#endif
//	asave = future[FSNOVA];
//	future[FSNOVA] = 1e30; /* defer supernovas */
	events();	/* Used to avoid if future[FSCMOVE] within time */
//	future[FSNOVA] = asave;
	/*fails if game over, quadrant super-novas or we've moved to new quadrant*/
	if (alldone || d.galaxy[quadx][quady] == 1000 || justin != 0) return 1;
	return 0;
}

void preport(void) {
	int iknow = 0, i;
	skip(1);
	chew();
	/* prout("Spock-  \"Planet report follows, Captain.\""); */
	prout("����-  \"���� �� ������, ����⠭.\"");
	skip(1);
	for (i = 1; i <= inplan; i++) {
		if (d.plnets[i].known
#ifdef DEBUG
			|| ( idebug && d.plnets[i].x !=0)
#endif
			) {
			iknow = 1;
#ifdef DEBUG
			if (idebug && d.plnets[i].known==0) proutn("(Unknown) ");
#endif
			cramlc(1, d.plnets[i].x, d.plnets[i].y);
			proutn("   ����� ");/*class*/
			proutn(classes[d.plnets[i].pclass]);
			proutn("   ");
			if (d.plnets[i].crystals == 0) proutn("��� ");/*no*/
			/* prout("dilithium crystals present."); */
			prout("����⨥��� ���⠫��� �����㦥��.");
			if (d.plnets[i].known==2) 
				/* prout("    Shuttle Craft Galileo on surface."); */
				prout("    ���� ������� �� �����孮��.");
		}
	}
	/* if (iknow==0) prout("No information available."); */
	if (iknow==0) prout("���ଠ�� ���������.");
}

void orbit(void) {
	double asave;

	skip(1);
	chew();
	ididit=0;
	if (inorbit!=0) {
		/* prout("Already in standard orbit."); */
		prout("��� ��室���� �� �⠭���⭮� �ࡨ�.");
		return;
	}
	if (damage[DWARPEN] != 0 && damage[DIMPULS] != 0) {
		/* prout("Both warp and impulse engines damaged."); */
		prout("���०���� � ���- � ������� �����⥫�.");
		return;
	}
	if (plnetx == 0 || abs(sectx-plnetx) > 1 || abs(secty-plnety) > 1) {
		crmshp();
		/* prout(" not adjacent to planet.\n"); */
		prout(" �� �冷� � �����⮩.\n");
		return;
	}
	Time = 0.02+0.03*Rand();
	/* prout("Helmsman Sulu-  \"Entering standard orbit, Sir.\""); */
	prout("�㫥��� ���-  \"��室�� �� �⠭������ �ࡨ��, ���.\"");
	newcnd();
	if (consumeTime()) return;
	/* proutn("Sulu-  \"Entered orbit at altitude "); */
	proutn("���-  \"��諨 �� �ࡨ�� �� ���� ");
	cramf(height = (1400.+7200.*Rand()), 0, 2);
	prout(" �������஢.\"");/*kilometers*/
	inorbit = 1;
	return;
}

void sensor(void) {
	skip(1);
	chew();
	if (damage[DSRSENS] != 0.0) {
		/* prout("Short range sensors damaged."); */
		prout("������ �������� ࠤ��� ����⢨� ���०����.");
		return;
	}
	if (plnetx == 0) {
		/* prout("No planet in this quadrant."); */
		prout("� �⮬ ����࠭� ��� ������.");
		return;
	}
	/* proutn("Spock-  \"Sensor scan for"); */
	proutn("����-  \"�����஢����");
	cramlc(1, quadx, quady);
	prout("-");
	skip(1);
	/* proutn("         Planet at"); */
	proutn("         ������ �");
	cramlc(2, plnetx, plnety);
	/* proutn(" is of class "); */
	proutn(" ����� ");
	proutn(classes[d.plnets[iplnet].pclass]);
	prout(".");
	if (d.plnets[iplnet].known==2) 
		/* prout("         Sensors show Galileo still on surface."); */
		prout("         ������ �����뢠��, �� ������� �� �� �����孮��.");
	/* proutn("         Readings indicate"); */
	proutn("         ����祭�� ����� �����뢠��");
	if (d.plnets[iplnet].crystals == 0) proutn(" ���"); /*no*/
	else proutn(" ����稥");
	/* prout(" dilithium crystals present.\""); */
	prout(" ����⨥��� ���⠫���.\"");
	if (d.plnets[iplnet].known == 0) d.plnets[iplnet].known = 1;
	return;
}

void beam(void) {
	chew();
	skip(1);
	if (damage[DTRANSP] != 0) {
		/* prout("Transporter damaged."); */
		prout("�࠭ᯮ���� ���०���.");
		if (damage[DSHUTTL]==0 && (d.plnets[iplnet].known==2 || iscraft == 1)) {
			skip(1);
			/* prout("Spock-  \"May I suggest the shuttle craft, Sir?\" "); */
			prout("����-  \"���� �।������ �ᯮ�짮���� ���, ���?\" ");
			if (ja() != 0) shuttle();
		}
		return;
	}
	if (inorbit==0) {
		crmshp();
		/* prout(" not in standard orbit."); */
		prout(" �� ��室���� �� �⠭���⭮� �ࡨ�.");
		return;
	}
	if (shldup!=0) {
		/* prout("Impossible to transport through shields."); */
		prout("�࠭ᯮ��஢�� ���������� ᪢��� ���.");
		return;
	}
	if (d.plnets[iplnet].known==0) {
/* 		prout("Spock-  \"Captain, we have no information on this planet");
		prout("  and Starfleet Regulations clearly state that in this situation");
		prout("  you may not go down.\"");
 */		prout("����-  \"����⠭, � ��� ��� ���ଠ樨 �� ������ ������");
		prout("  � ��⠢ ��������� ���� �������筮 ����뢠�� ������ ����� - ");
		prout("  ��ᠤ�� ����饭�.\"");
		return;
	}
	if (landed==1) {
		/* Coming from planet */
		if (d.plnets[iplnet].known==2) {
			/* proutn("Spock-  \"Wouldn't you rather take the Galileo?\" "); */
			proutn("����-  \"����� �� ���쬥� �������?\" ");
			if (ja() != 0) {
				chew();
				return;
			}
			/* prout("Your crew hides the Galileo to prevent capture by aliens."); */
			prout("��� ������� ���⠫� ������� �� ���ਣ����.");
		}
/* 		prout("Landing party assembled, ready to beam up."); 
 		skip(1);
		prout("Kirk whips out communicator...");
		prouts("BEEP  BEEP  BEEP");
		skip(2);
		prout("\"Kirk to enterprise-  Lock on coordinates...energize.\"");
 */		prout("��㯯� ��ᠤ�� ��⮢� � �࠭ᯮ��஢��.");
		skip(1);
		prout("��� ����稫 ����㭨����...");
		prouts("����  ����  ����");
		skip(2);
		prout("\"��� - ����ࠩ�� -  ��墠� �� ���न��⠬...����砩�.\"");
	}
	else {
		/* Going to planet */
		if (d.plnets[iplnet].crystals==0) {
/* 			prout("Spock-  \"Captain, I fail to see the logic in");
			prout("  exploring a planet with no dilithium crystals.");
			proutn("  Are you sure this is wise?\" ");
 */			prout("����-  \"����⠭, �� ���� ������� ������ � ⮬,");
			prout("  �⮡� ��᫥������ ������� ��� ����⨥��� ���⠫���.");
			proutn("  �� 㢥७�, �� �� ���?\" ");
			if (ja()==0) {
				chew();
				return;
			}
		}
/* 		prout("Scotty-  \"Transporter room ready, Sir.\"");
		skip(1);
		prout("Kirk, and landing party prepare to beam down to planet surface.");
		skip(1);
		prout("Kirk-  \"Energize.\"");
 */		prout("�����-  \"�࠭ᯮ���ୠ� ��⮢�, ���.\"");
		skip(1);
		prout("��� � ��㯯� ��ᠤ�� ��⮢� � �࠭ᯮ��樨 �� �����孮���.");
		skip(1);
		prout("���-  \"����砩.\"");
	}
	skip(1);
	/* prouts("WWHOOOIIIIIRRRRREEEE.E.E.  .  .  .  .   .    ."); */
	prouts("�����������������.�.�.  .  .  .  .   .    .");
	skip(2);
	if (Rand() > 0.98) {
		/* prouts("BOOOIIIOOOIIOOOOIIIOIING . . ."); */
		prouts("������������� . . .����");
		skip(2);
		/* prout("Scotty-  \"Oh my God!  I've lost them.\""); */
		prout("�����-  \"���� ���!  � ����� ��.\"");
		finish(FLOST);
		return;
	}
	/* prouts(".    .   .  .  .  .  .E.E.EEEERRRRRIIIIIOOOHWW"); */
	prouts(".    .   .  .  .  .  .�.�.�.�������������������������");
	skip(2);
	prout("�࠭ᯮ��஢�� �����襭�.");/*Transport complete*/
	landed = -landed;
	if (landed==1 && d.plnets[iplnet].known==2) {
		/* prout("The shuttle craft Galileo is here!"); */
		prout("���� ������� �� ����!");
	}
	if (landed!=1 && imine==1) {
		icrystl = 1;
		cryprob = 0.05;
	}
	imine = 0;
	return;
}

void mine(void) {

	ididit = 0;
	skip(1);
	chew();
	if (landed!= 1) {
		/* prout("Mining party not on planet."); */
		prout("������ �� ��室���� �� ������.");
		return;
	}
	if (d.plnets[iplnet].crystals == 0) {
		/* prout("No dilithium crystals on this planet."); */
		prout("�� ������ ��� ����⨥��� ���⠫���.");
		return;
	}
	if (imine == 1) {
		/* prout("You've already mined enough crystals for this trip."); */
		prout("�� 㦥 ���뫨 ���ᨬ� ���⠫��� ��� ������ ३�.");
		return;
	}
	if (icrystl == 1 && cryprob == 0.05) {
		/* proutn("With all those fresh crystals aboard the "); */
		proutn("� ��� ����� ᢥ��� ���⠫��� �� ����� ");
		crmshp();
		skip(1);
		/* prout("there's no reason to mine more at this time."); */
		prout("��� �������� ��᫠ ���뢠�� ��.");
		return;
	}
	Time = (0.1+0.2*Rand())*d.plnets[iplnet].pclass;
	if (consumeTime()) return;
	/* prout("Mining operation complete."); */
	prout("������ ����� ���⠫��� �����襭�.");
	imine = 1;
	return;
}

void usecrystals(void) {

	skip(1);
	chew();
	if (icrystl!=1) {
		/* prout("No dilithium crystals available."); */
		prout("����⨥�� ���⠫�� ����������.");
		return;
	}
	if (energy >= 1000) {
/* 		prout("Spock-  \"Captain, Starfleet Regulations prohibit such an operation");
		prout("  except when condition Yellow exists.");
 */		prout("����-  \"����⠭, ��⠢ ��������� ���� ����頥� ⠪�� ����樨");
		prout("  �஬� ��� �� ���⮬ ���� ���᭮��.");
		return;
	}
/* 	prout("Spock- \"Captain, I must warn you that loading");
	prout("  raw dilithium crystals into the ship's power");
	prout("  system may risk a severe explosion.");
	proutn("  Are you sure this is wise?\" ");
 */	prout("����- \"����⠭, � ������ �।�।��� ��� - ");
	prout("  ����㧪� � �࣮��⥬� ����ࠡ�⠭��� ����⨥���");
	prout("  ���⠫��� ����� �ਢ��� � �����.");
	proutn("  �� 㢥७�?\" ");
	if (ja()==0) {
		chew();
		return;
	}
	skip(1);
/* 	prout("Engineering Officer Scott-  \"(GULP) Aye Sir.");
	prout("  Mr. Spock and I will try it.\"");
	skip(1);
	prout("Spock-  \"Crystals in place, Sir.");
	prout("  Ready to activate circuit.\"");
	skip(1);
	prouts("Scotty-  \"Keep your fingers crossed, Sir!\"");
 */	prout("�������-���� �����  \"(������) ��� �筮, ���.");
	prout("  ����� ���� � � ���஡㥬 ᤥ���� ��.\"");
	skip(1);
	prout("����-  \"���⠫�� ����㦥��, ���.");
	prout("  ��⮢� � ��⨢�樨 ॠ���.\"");
	skip(1);
	prouts("�����-  \"������ ������ �� 㤠��, ���!\"");
	skip(1);
	if (Rand() <= cryprob) {
		/* prouts("  \"Activating now! - - No good!  It's***"); */
		prouts("  \"��⨢��� �ந�������! - - �� � �� ⠪!  �� ��***");
		skip(2);
		/* prouts("***RED ALERT!  RED A*L********************************"); */
		prouts("***���������!  ���*���********************************");
		skip(1);
		stars();
		/* prouts("******************   KA-BOOM!!!!   *******************"); */
		prouts("******************   ��-���!!!!   *******************");
		skip(1);
		kaboom();
		return;
	}
	energy += 5000.0*(1.0 + 0.9*Rand());
/* 	prouts("  \"Activating now! - - ");
	prout("The instruments");
	prout("   are going crazy, but I think it's");
	prout("   going to work!!  Congratulations, Sir!\"");
 */	prouts("  \"��⨢��� �ந�������! - - ");
	prout("������ �ਡ�஢");
	prout("   �ᡥᨫ���, �� � �㬠�, �� ��");
	prout("   �㤥� ࠡ����!!  ����ࠢ���, ���!\"");
	cryprob *= 2.0;
	return;
}

void shuttle(void) {

	chew();
	skip(1);
	ididit = 0;
	if(damage[DSHUTTL] != 0.0) {
		if (damage[DSHUTTL] == -1.0) {
			if (inorbit && d.plnets[iplnet].known == 2)
				/* prout("Ye Faerie Queene has no shuttle craft bay to dock it at."); */
				prout("��஫��� ��� �� ����� ��ᠤ�筮�� ��ᥪ� ��� ��⫠.");
			else
				/* prout("Ye Faerie Queene had no shuttle craft."); */
				prout("�� ��஫��� ��� ��� ��⫠.");
		}
		else if (damage[DSHUTTL] > 0)
			/* prout("The Galileo is damaged."); */
			prout("������� �஢०���.");
		/* else prout("Shuttle craft is now serving Big Mac's."); */
		else prout("���� ᥩ�� ࠧ����� �������.");
		return;
	}
	if (inorbit==0) {
		crmshp();
		/* prout(" not in standard orbit."); */
		prout(" �� ��室���� �� �⠭���⭮� �ࡨ�.");
		return;
	}
	if ((d.plnets[iplnet].known != 2) && iscraft != 1) {
		/* prout("Shuttle craft not currently available."); */
		prout("���� � ����� ������ ������㯥�.");
		return;
	}
	if (landed==-1 && d.plnets[iplnet].known==2) {
		/* prout("You will have to beam down to retrieve the shuttle craft."); */
		prout("��� �㦭� ��⨢�஢��� ᨫ���� ���, �⮡� ������� ��� � �����孮��.");
		return;
	}
	if (shldup!=0 || condit == IHDOCKED) {
		/* prout("Shuttle craft cannot pass through shields."); */
		prout("���� ���������� �࠭ᯮ��஢��� ᪢��� ���.");
		return;
	}
	if (d.plnets[iplnet].known==0) {
/* 		prout("Spock-  \"Captain, we have no information on this planet");
		prout("  and Starfleet Regulations clearly state that in this situation");
		prout("  you may not fly down.\"");
 */		prout("����-  \"����⠭, � ��� ��� ���ଠ樨 �� ������ ������");
		prout("  � ��⠢ ��������� ���� �������筮 ����뢠�� ������ ����� - ");
		prout("  ��ᠤ�� ����饭�.\"");
		return;
	}
	Time = 3.0e-5*height;
	if (Time >= 0.8*d.remtime) {
/* 		prout("First Officer Spock-  \"Captain, I compute that such");
		prout("  a maneuver would require approximately ");
 */		prout("���� ���� ����-  \"����⠭, � ����⠫, �� ���");
		prout("  ������ ������ �ਡ����⥫쭮 ");
		cramf(100*Time/d.remtime,0,4);
/* 		prout("% of our");
		prout("remaining time.");
		prout("Are you sure this is wise?\" ");
 */		prout("% �� ��襣�");
		prout("��⠢襣��� �६���.");
		prout("�� 㢥७�?\" ");
		if (ja()==0) {
			Time = 0.0;
			return;
		}
	}
	if (landed == 1) {
		/* Kirk on planet */
		if (iscraft==1) {
			/* Galileo on ship! */
			if (damage[DTRANSP]==0) {
				/* proutn("Spock-  \"Would you rather use the transporter?\" "); */
				proutn("����-  \"�����, ���� �ᯮ�짮���� �࠭ᯮ����?\" ");
				if (ja() != 0) {
					beam();
					return;
				}
				proutn("������� ��⫠");/*Shuttle crew*/
			}
			else
				proutn("����⥫쭠� �������");/*Rescue party*/
			/* prout(" boards Galileo and swoops toward planet surface."); */
			prout(" ᥫ� �� ������� � ��ࠢ������ �� �����孮���.");
			iscraft = 0;
			skip(1);
			if (consumeTime()) return;
			d.plnets[iplnet].known=2;
			prout("��ᠤ�� �����襭�.");/*Trip complete*/
			return;
		}
		else {
			/* Ready to go back to ship */
/* 			prout("You and your mining party board the");
			prout("shuttle craft for the trip back to the Enterprise.");
			skip(1);
			prout("The short hop begins . . .");
 */			prout("�� � ���⨥� ���஢ ᥫ� � ���");
			prout("��� �����饭�� �� ����ࠩ�.");
			skip(1);
			/* prout("The short hop begins . . ."); */
			prout("����� ����� ��襫 . . .");
			d.plnets[iplnet].known=1;
			icraft = 1;
			skip(1);
			landed = -1;
			if (consumeTime()) return;
			iscraft = 1;
			icraft = 0;
			if (imine!=0) {
				icrystl = 1;
				cryprob = 0.05;
			}
			imine = 0;
			/* prout("Trip complete."); */
			prout("����� �����襭.");
			return;
		}
	}
	else {
		/* Kirk on ship */
		/* and so is Galileo */
/* 		prout("Mining party assembles in the hangar deck,");
		prout("ready to board the shuttle craft \"Galileo\".");
		skip(1);
		prouts("The hangar doors open; the trip begins.");
 */		prout("����� ���஢ ᮡࠫ��� � �����,");
		prout("��⮢� � ��ᠤ�� �� ��� \"�������\".");
		skip(1);
		prouts("���� ����� ���뢠����; ����� ��稭�����.");
		skip(1);
		icraft = 1;
		iscraft = 0;
		if (consumeTime()) return;
		d.plnets[iplnet].known = 2;
		landed = 1;
		icraft = 0;
		prout("��ᠤ�� �����襭�");
		return;
	}
}
		

void deathray(void) {
	double r = Rand();
	
	ididit = 0;
	skip(1);
	chew();
	if (ship != IHE) {
		/* prout("Ye Faerie Queene has no death ray."); */
		prout("�� ��஫��� ��� ��� ��� �����.");
		return;
	}
	if (nenhere==0) {
		/* prout("Sulu-  \"But Sir, there are no enemies in this quadrant.\""); */
		prout("���-  \"�� ���, � �⮬ ����࠭� �ࠣ� ����������.\"");
		return;
	}
	if (damage[DDRAY] > 0.0) {
		/* prout("Death Ray is damaged."); */
		prout("��� ����� ���०���.");
		return;
	}
/* 	prout("Spock-  \"Captain, the 'Experimental Death Ray'");
	prout("  is highly unpredictable.  Considering the alternatives,");
	prout("  are you sure this is wise?\" ");
 */	prout("����-  \"����⠭, '��ᯥਬ��⠫�� ��� �����'");
	prout("  ���쬠 ���।᪠�㥬.  ���ᬮ��� ����ୠ⨢�,");
	prout("  �� 㢥७� � ࠧ㬭��� �⮣�?\" ");
	if (ja()==0) return;
	/* prout("Spock-  \"Acknowledged.\""); */
	prout("����-  \"���⢥থ��� �ਭ��.\"");
	skip(1);
	ididit=1;
/* 	prouts("WHOOEE ... WHOOEE ... WHOOEE ... WHOOEE"); 
	skip(1);
	prout("Crew scrambles in emergency preparation.");
	prout("Spock and Scotty ready the death ray and");
	prout("prepare to channel all ship's power to the device.");
	skip(1);
	prout("Spock-  \"Preparations complete, sir.\"");
	prout("Kirk-  \"Engage!\"");
	skip(1);
	prouts("WHIRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR");
 */	prouts("������ ... ������ ... ������ ... ������");
	skip(1);
	prout("������� �ਣ�⮢����� �� ���਩���� ��⮪���.");
	prout("���� � ����� �ਣ�⮢��� ��� ����� �");
	prout("�ਣ�⮢��� ����� ����� �ࣨ� �� ���ன�⢮.");
	skip(1);
	prout("����-  \"�����⮢�� ��� �����襭�, ���.\"");
	prout("���-  \"����砩!\"");
	skip(1);
	prouts("����������������������������������������������");
	skip(1);
	if (r > .30) {
		/* prouts("Sulu- \"Captain!  It's working!\""); */
		prouts("���- \"����⠭!  �� �ࠡ�⠫!\"");
		skip(2);
		while (nenhere > 0) {
			deadkl(kx[1],ky[1],quad[kx[1]][ky[1]],kx[1],ky[1]);
		}
		/* prout("Ensign Chekov-  \"Congratulations, Captain!\""); */
		prout("��笠� ��客-  \"����ࠢ�����, ����⠭!\"");
		if (d.remkl == 0) finish(FWON);
		/* prout("Spock-  \"Captain, I believe the `Experimental Death Ray'"); */
		prout("����-  \"����⠭, � �㬠�, `��ᯥਬ��⠫�� ��� �����'");
		if (Rand() <= 0.05) {
			/* prout("   is still operational.\""); */
			prout("   ��������� ࠡ��ᯮᮡ��.\"");
		}
		else {
			/* prout("   has been rendered dysfunctional.\""); */
			prout("   ��襫 �� ����.\"");
			damage[DDRAY] = 39.95;
		}
		return;
	}
	r = Rand();	// Pick failure method 
	if (r <= .30) {
/* 		prouts("Sulu- \"Captain!  It's working!\"");
		skip(1);
		prouts("***RED ALERT!  RED ALERT!");
		skip(1);
		prout("***MATTER-ANTIMATTER IMPLOSION IMMINENT!");
		skip(1);
		prouts("***RED ALERT!  RED A*L********************************");
		skip(1);
		stars();
		prouts("******************   KA-BOOM!!!!   *******************");
 */		prouts("���- \"����⠭!  ��� ࠡ�⠥�!\"");
		skip(1);
		prouts("***���������!  ���������!");
		skip(1);
		prout("***��������� ����� �����������!");
		skip(1);
		prouts("***���������!  ���*���********************************");
		skip(1);
		stars();
		prouts("******************   ��-���!!!!   *******************");
		skip(1);
		kaboom();
		return;
	}
	if (r <= .55) {
/* 		prouts("Sulu- \"Captain!  Yagabandaghangrapl, brachriigringlanbla!\"");
		skip(1);
		prout("Lt. Uhura-  \"Graaeek!  Graaeek!\"");
		skip(1);
		prout("Spock-  \"Fascinating!  . . . All humans aboard");
		prout("  have apparently been transformed into strange mutations.");
		prout("  Vulcans do not seem to be affected.");
		skip(1);
		prout("Kirk-  \"Raauch!  Raauch!\"");
 */		prouts("���- \"����⠭!  ���������堠�࠯�, �࠭�ਨ�ਭ������!\"");
		skip(1);
		prout("���⥭��� ����-  \"�ࠠ���!  �ࠠ���!\"");
		skip(1);
		prout("����-  \"�����������!  . . . �� � �� �����");
		prout("  �������� �����࣫��� ��࠭�� �����.");
		prout("  �㫪���� �� �����থ�� ⠪��� �������⢨� ���.");
		skip(1);
		prout("���-  \"�����!  �����!\"");
		finish(FDRAY);
		return;
	}
	if (r <= 0.75) {
		int i,j;
/* 		prouts("Sulu- \"Captain!  It's   --WHAT?!?!\"");
		skip(2);
		proutn("Spock-  \"I believe the word is");
		prouts(" *ASTONISHING*");
		prout(" Mr. Sulu.");
 */		prouts("���- \"����⠭!  ��   --���?!?!\"");
		skip(2);
		proutn("����-  \"� �㬠� ���室�饥 ᫮��");
		prouts(" *�����������*");
		prout(" ����� ���.");
		for (i=1; i<=10; i++)
			for (j=1; j<=10; j++)
				if (quad[i][j] == IHDOT) quad[i][j] = IHQUEST;
/* 		prout("  Captain, our quadrant is now infested with");
		prouts(" - - - - - -  *THINGS*.");
		skip(1);
		prout("  I have no logical explanation.\"");
 */		prout("  ����⠭, ��� ����࠭� ��ᥫ��");
		prouts(" - - - - - -  *������*.");
		skip(1);
		prout("  � ���� ��� �����᪮�� ����᭥���.\"");
		return;
	}
/* 	prouts("Sulu- \"Captain!  The Death Ray is creating tribbles!\"");
	skip(1);
	prout("Scotty-  \"There are so many tribbles down here");
	prout("  in Engineering, we can't move for 'em, Captain.\"");
 */	prouts("���- \"����⠭!  ��� ����� ᮧ��� �ਡ����!\"");
	skip(1);
	prout("�����-  \"�� � �� ᫨誮� � �����");
	prout("  � ������୮� ��ᥪ�, �� �� ����� ��������� ��-�� ���, ����⠭.\"");
	finish(FTRIBBLE);
	return;
}
