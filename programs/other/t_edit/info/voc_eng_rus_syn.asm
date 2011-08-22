macro wo txt,lf1,p1,p2,p3{
@@: db txt
rb @b+40-$
dd lf1
db p1
dw p2+0
db p3
}
count_colors_text dd (text-color_wnd_text)/4
count_key_words dd (f1-text)/48
color_cursor dd 0xffd000
color_wnd_capt dd 0x004080
color_wnd_work dd 0xffffff
color_wnd_bord dd 0x00ffff
color_select dd 0xc0c0c0
color_cur_text dd 0xc0c0c0
color_wnd_text:
	dd 0x000000
	dd 0x0080ff
	dd 0xff0000
	dd 0x00c000
text:
wo<'',9,''>,0,0,,2
wo<'',13,''>,0,0,,2
wo<' '>,0,0,,2
wo<'('>,0,0,,2
wo<')'>,0,0,,2
wo<','>,0,0,,2
wo<'-'>,0,0,,2
wo<'.'>,0,0,,2
wo<';'>,0,0,,2
wo<'<'>,0,0,,2
wo<'>'>,0,0,,2
wo<'ABC'>,f1.12-f1,3,,1
wo<'Academy'>,f1.13-f1,3,,1
wo<'Accra'>,f1.14-f1,3,,1
wo<'Addis Ababa'>,f1.15-f1,3,,1
wo<'Afghan'>,f1.16-f1,3,,1
wo<'Afghanistan'>,f1.17-f1,3,,1
wo<'Africa'>,f1.18-f1,3,,1
wo<'African'>,f1.19-f1,3,,1
wo<'Alabama'>,f1.20-f1,3,,1
wo<'Aland Islands'>,f1.21-f1,3,,1
wo<'Alaska'>,f1.22-f1,3,,1
wo<'Albania'>,f1.23-f1,3,,1
wo<'Albanian'>,f1.24-f1,3,,1
wo<'Algeria'>,f1.25-f1,3,,1
wo<'Algiers'>,f1.26-f1,3,,1
wo<'Alps'>,f1.27-f1,3,,1
wo<'Amazon'>,f1.28-f1,3,,1
wo<'America'>,f1.29-f1,3,,1
wo<'American'>,f1.30-f1,3,,1
wo<'Amman'>,f1.31-f1,3,,1
wo<'Amsterdam'>,f1.32-f1,3,,1
wo<'Ankara'>,f1.33-f1,3,,1
wo<'Antarctic'>,f1.34-f1,3,,1
wo<'Appennines'>,f1.35-f1,3,,1
wo<'April'>,f1.36-f1,3,,1
wo<'Arctic'>,f1.37-f1,3,,1
wo<'Arctic Ocean'>,f1.38-f1,3,,1
wo<'Argentina'>,f1.39-f1,3,,1
wo<'Arizona'>,f1.40-f1,3,,1
wo<'Arkansas'>,f1.41-f1,3,,1
wo<'Asia'>,f1.42-f1,3,,1
wo<'Asiatic'>,f1.43-f1,3,,1
wo<'Athens'>,f1.44-f1,3,,1
wo<'Atlantic Ocean'>,f1.45-f1,3,,1
wo<'August'>,f1.46-f1,3,,1
wo<'Australia'>,f1.47-f1,3,,1
wo<'Australian'>,f1.48-f1,3,,1
wo<'Austria'>,f1.49-f1,3,,1
wo<'Belgrade'>,f1.50-f1,3,,1
wo<'British'>,f1.51-f1,3,,1
wo<'Brussels'>,f1.52-f1,3,,1
wo<'Bucharest'>,f1.53-f1,3,,1
wo<'Cairo'>,f1.54-f1,3,,1
wo<'Calcutta'>,f1.55-f1,3,,1
wo<'California'>,f1.56-f1,3,,1
wo<'Cambridge'>,f1.57-f1,3,,1
wo<'Canada'>,f1.58-f1,3,,1
wo<'Canberra'>,f1.59-f1,3,,1
wo<'Canterbury'>,f1.60-f1,3,,1
wo<'Ceylon'>,f1.61-f1,3,,1
wo<'Cherkasy'>,f1.62-f1,3,,1
wo<'Chernihiv'>,f1.63-f1,3,,1
wo<'Chernivtsi'>,f1.64-f1,3,,1
wo<'Chile'>,f1.65-f1,3,,1
wo<'China'>,f1.66-f1,3,,1
wo<'Chinese'>,f1.67-f1,3,,1
wo<'Christmas'>,f1.68-f1,3,,1
wo<'Colombo'>,f1.69-f1,3,,1
wo<'Conakry'>,f1.70-f1,3,,1
wo<'Copenhagen'>,f1.71-f1,3,,1
wo<'Cordilleras'>,f1.72-f1,3,,1
wo<'Cornwall'>,f1.73-f1,3,,1
wo<'Cuba'>,f1.74-f1,3,,1
wo<'December'>,f1.75-f1,3,,1
wo<'Democratic Republic of Vietnam'>,f1.76-f1,3,,1
wo<'Dnipropetrovsk'>,f1.77-f1,3,,1
wo<'Donetsk'>,f1.78-f1,3,,1
wo<'Dutch'>,f1.79-f1,3,,1
wo<'Dutchman'>,f1.80-f1,3,,1
wo<'Easter'>,f1.81-f1,3,,1
wo<'England'>,f1.82-f1,3,,1
wo<'English'>,f1.83-f1,3,,1
wo<'Europe'>,f1.84-f1,3,,1
wo<'February'>,f1.85-f1,3,,1
wo<'French'>,f1.86-f1,3,,1
wo<'Friday'>,f1.87-f1,3,,1
wo<'German'>,f1.88-f1,3,,1
wo<'Ghana'>,f1.89-f1,3,,1
wo<'Gibraltar'>,f1.90-f1,3,,1
wo<'Gipsy'>,f1.91-f1,3,,1
wo<'Great Britain'>,f1.92-f1,3,,1
wo<'Great Patriotic War'>,f1.93-f1,3,,1
wo<'Greece'>,f1.94-f1,3,,1
wo<'Himalaya'>,f1.95-f1,3,,1
wo<'Himalayas'>,f1.96-f1,3,,1
wo<'Hindu'>,f1.97-f1,3,,1
wo<'I'>,f1.98-f1,3,,1
wo<'I am anxious about children'>,f1.99-f1,3,,1
wo<'I am aware'>,f1.100-f1,3,,1
wo<'I am sorry'>,f1.101-f1,3,,1
wo<'I can see'>,f1.102-f1,3,,1
wo<'I',39,'ll attend to luggage'>,f1.103-f1,3,,1
wo<'I',39,'m all alone'>,f1.104-f1,3,,1
wo<'I',39,'m alone'>,f1.105-f1,3,,1
wo<'Israel'>,f1.106-f1,3,,1
wo<'Ivano-Frankivsk'>,f1.107-f1,3,,1
wo<'January'>,f1.108-f1,3,,1
wo<'Japan'>,f1.109-f1,3,,1
wo<'July'>,f1.110-f1,3,,1
wo<'June'>,f1.111-f1,3,,1
wo<'Kabul'>,f1.112-f1,3,,1
wo<'Kansas'>,f1.113-f1,3,,1
wo<'Karachi'>,f1.114-f1,3,,1
wo<'Kentucky'>,f1.115-f1,3,,1
wo<'Kharkiv'>,f1.116-f1,3,,1
wo<'Kherson'>,f1.117-f1,3,,1
wo<'Khmelnytsky'>,f1.118-f1,3,,1
wo<'Kirovohrad'>,f1.119-f1,3,,1
wo<'Kyiv'>,f1.120-f1,3,,1
wo<'Latin'>,f1.121-f1,3,,1
wo<'Luhansk'>,f1.122-f1,3,,1
wo<'Lutsk'>,f1.123-f1,3,,1
wo<'Lviv'>,f1.124-f1,3,,1
wo<'March'>,f1.125-f1,3,,1
wo<'May'>,f1.126-f1,3,,1
wo<'Minnesota'>,f1.127-f1,3,,1
wo<'Monday'>,f1.128-f1,3,,1
wo<'Mykolaiv'>,f1.129-f1,3,,1
wo<'New Year tree'>,f1.130-f1,3,,1
wo<'Nile'>,f1.131-f1,3,,1
wo<'November'>,f1.132-f1,3,,1
wo<'October'>,f1.133-f1,3,,1
wo<'Odesa'>,f1.134-f1,3,,1
wo<'Ohio'>,f1.135-f1,3,,1
wo<'Oklahoma'>,f1.136-f1,3,,1
wo<'Olympic'>,f1.137-f1,3,,1
wo<'Oregon'>,f1.138-f1,3,,1
wo<'Oslo'>,f1.139-f1,3,,1
wo<'Ottawa'>,f1.140-f1,3,,1
wo<'Pakistan'>,f1.141-f1,3,,1
wo<'Panama'>,f1.142-f1,3,,1
wo<'Paris'>,f1.143-f1,3,,1
wo<'People',39,'s Republic of Albania'>,f1.144-f1,3,,1
wo<'Poltava'>,f1.145-f1,3,,1
wo<'Quebec'>,f1.146-f1,3,,1
wo<'Rivne'>,f1.147-f1,3,,1
wo<'Roman'>,f1.148-f1,3,,1
wo<'Rome'>,f1.149-f1,3,,1
wo<'Russia'>,f1.150-f1,3,,1
wo<'Russian'>,f1.151-f1,3,,1
wo<'Saturday'>,f1.152-f1,3,,1
wo<'Scotland'>,f1.153-f1,3,,1
wo<'Scottish'>,f1.154-f1,3,,1
wo<'September'>,f1.155-f1,3,,1
wo<'Simferopol'>,f1.156-f1,3,,1
wo<'Sumy'>,f1.157-f1,3,,1
wo<'Sunday'>,f1.158-f1,3,,1
wo<'TV-set'>,f1.159-f1,3,,1
wo<'Tel Aviv'>,f1.160-f1,3,,1
wo<'Ternopil'>,f1.161-f1,3,,1
wo<'Thames'>,f1.162-f1,3,,1
wo<'Thursday'>,f1.163-f1,3,,1
wo<'Tuesday'>,f1.164-f1,3,,1
wo<'Turkish'>,f1.165-f1,3,,1
wo<'Tyrk'>,f1.166-f1,3,,1
wo<'Tyrkey'>,f1.167-f1,3,,1
wo<'Ukraine'>,f1.168-f1,3,,1
wo<'Ukrainian'>,f1.169-f1,3,,1
wo<'Ulan Bator'>,f1.170-f1,3,,1
wo<'Union of Soviet Socialist Republics'>,f1.171-f1,3,,1
wo<'Uzhhorod'>,f1.172-f1,3,,1
wo<'Vermont'>,f1.173-f1,3,,1
wo<'Vienna'>,f1.174-f1,3,,1
wo<'Vietnam'>,f1.175-f1,3,,1
wo<'Vinnytsia'>,f1.176-f1,3,,1
wo<'Virginia'>,f1.177-f1,3,,1
wo<'Volgograd'>,f1.178-f1,3,,1
wo<'Wednesday'>,f1.179-f1,3,,1
wo<'Wellington'>,f1.180-f1,3,,1
wo<'West Virginia'>,f1.181-f1,3,,1
wo<'X-rays'>,f1.182-f1,3,,1
wo<'Zaporizhzhia'>,f1.183-f1,3,,1
wo<'Zhytomyr'>,f1.184-f1,3,,1
wo<'a certain'>,f1.185-f1,3,,1
wo<'abandon'>,f1.186-f1,3,,1
wo<'abbreviation'>,f1.187-f1,3,,1
wo<'ability'>,f1.188-f1,3,,1
wo<'able'>,f1.189-f1,3,,1
wo<'abnormal'>,f1.190-f1,3,,1
wo<'aboard'>,f1.191-f1,3,,1
wo<'abolish'>,f1.192-f1,3,,1
wo<'abolition'>,f1.193-f1,3,,1
wo<'abominable'>,f1.194-f1,3,,1
wo<'abound'>,f1.195-f1,3,,1
wo<'abound in'>,f1.196-f1,3,,1
wo<'about'>,f1.197-f1,3,,1
wo<'above'>,f1.198-f1,3,,1
wo<'abridge'>,f1.199-f1,3,,1
wo<'abroad'>,f1.200-f1,3,,1
wo<'abrupt'>,f1.201-f1,3,,1
wo<'abscess'>,f1.202-f1,3,,1
wo<'absence'>,f1.203-f1,3,,1
wo<'absent'>,f1.204-f1,3,,1
wo<'absent-minded'>,f1.205-f1,3,,1
wo<'absolute'>,f1.206-f1,3,,1
wo<'absolutely'>,f1.207-f1,3,,1
wo<'absorb'>,f1.208-f1,3,,1
wo<'abstain'>,f1.209-f1,3,,1
wo<'abstract'>,f1.210-f1,3,,1
wo<'absurd'>,f1.211-f1,3,,1
wo<'absurdity'>,f1.212-f1,3,,1
wo<'abundant'>,f1.213-f1,3,,1
wo<'abuse'>,f1.214-f1,3,,1
wo<'accelerate'>,f1.215-f1,3,,1
wo<'accent'>,f1.216-f1,3,,1
wo<'accept'>,f1.217-f1,3,,1
wo<'access'>,f1.218-f1,3,,1
wo<'accessible'>,f1.219-f1,3,,1
wo<'accident'>,f1.220-f1,3,,1
wo<'accidental'>,f1.221-f1,3,,1
wo<'accommodation'>,f1.222-f1,3,,1
wo<'accompany'>,f1.223-f1,3,,1
wo<'accomplice'>,f1.224-f1,3,,1
wo<'accord'>,f1.225-f1,3,,1
wo<'accordance'>,f1.226-f1,3,,1
wo<'according'>,f1.227-f1,3,,1
wo<'according to'>,f1.228-f1,3,,1
wo<'accordingly'>,f1.229-f1,3,,1
wo<'account'>,f1.230-f1,3,,1
wo<'account for'>,f1.231-f1,3,,1
wo<'accountant'>,f1.232-f1,3,,1
wo<'accumulate'>,f1.233-f1,3,,1
wo<'accurate'>,f1.234-f1,3,,1
wo<'accusation'>,f1.235-f1,3,,1
wo<'accusative'>,f1.236-f1,3,,1
wo<'accuse'>,f1.237-f1,3,,1
wo<'accustom'>,f1.238-f1,3,,1
wo<'ache'>,f1.239-f1,3,,1
wo<'achieve'>,f1.240-f1,3,,1
wo<'achievement'>,f1.241-f1,3,,1
wo<'acid'>,f1.242-f1,3,,1
wo<'acknowledge'>,f1.243-f1,3,,1
wo<'acknowledgement'>,f1.244-f1,3,,1
wo<'acorn'>,f1.245-f1,3,,1
wo<'acquaintance'>,f1.246-f1,3,,1
wo<'acquire'>,f1.247-f1,3,,1
wo<'acquit'>,f1.248-f1,3,,1
wo<'acre'>,f1.249-f1,3,,1
wo<'across'>,f1.250-f1,3,,1
wo<'act'>,f1.251-f1,3,,1
wo<'acting'>,f1.252-f1,3,,1
wo<'action'>,f1.253-f1,3,,1
wo<'active'>,f1.254-f1,3,,1
wo<'active voice'>,f1.255-f1,3,,1
wo<'activity'>,f1.256-f1,3,,1
wo<'actor'>,f1.257-f1,3,,1
wo<'actress'>,f1.258-f1,3,,1
wo<'actual'>,f1.259-f1,3,,1
wo<'acute'>,f1.260-f1,3,,1
wo<'acute satisfaction'>,f1.261-f1,3,,1
wo<'adaptation'>,f1.262-f1,3,,1
wo<'add'>,f1.263-f1,3,,1
wo<'addition'>,f1.264-f1,3,,1
wo<'additional'>,f1.265-f1,3,,1
wo<'address'>,f1.266-f1,3,,1
wo<'adequate'>,f1.267-f1,3,,1
wo<'adherent'>,f1.268-f1,3,,1
wo<'adjacent'>,f1.269-f1,3,,1
wo<'adjective'>,f1.270-f1,3,,1
wo<'adjourn'>,f1.271-f1,3,,1
wo<'adjust'>,f1.272-f1,3,,1
wo<'administration'>,f1.273-f1,3,,1
wo<'admiral'>,f1.274-f1,3,,1
wo<'admiralty'>,f1.275-f1,3,,1
wo<'admiration'>,f1.276-f1,3,,1
wo<'admire'>,f1.277-f1,3,,1
wo<'admission'>,f1.278-f1,3,,1
wo<'admit'>,f1.279-f1,3,,1
wo<'admittance'>,f1.280-f1,3,,1
wo<'admitting that this is the case'>,f1.281-f1,3,,1
wo<'adolescent'>,f1.282-f1,3,,1
wo<'adopt'>,f1.283-f1,3,,1
wo<'adoption'>,f1.284-f1,3,,1
wo<'adore'>,f1.285-f1,3,,1
wo<'adorn'>,f1.286-f1,3,,1
wo<'adult'>,f1.287-f1,3,,1
wo<'advance'>,f1.288-f1,3,,1
wo<'advantage'>,f1.289-f1,3,,1
wo<'adventure'>,f1.290-f1,3,,1
wo<'adventurer'>,f1.291-f1,3,,1
wo<'adventurous'>,f1.292-f1,3,,1
wo<'adventurous journey'>,f1.293-f1,3,,1
wo<'adverb'>,f1.294-f1,3,,1
wo<'adversary'>,f1.295-f1,3,,1
wo<'adversity'>,f1.296-f1,3,,1
wo<'advertisement'>,f1.297-f1,3,,1
wo<'advice'>,f1.298-f1,3,,1
wo<'advise'>,f1.299-f1,3,,1
wo<'advocate'>,f1.300-f1,3,,1
wo<'aerial'>,f1.301-f1,3,,1
wo<'aerodrome'>,f1.302-f1,3,,1
wo<'aeroplane'>,f1.303-f1,3,,1
wo<'affair'>,f1.304-f1,3,,1
wo<'affect'>,f1.305-f1,3,,1
wo<'affectation'>,f1.306-f1,3,,1
wo<'affection'>,f1.307-f1,3,,1
wo<'affectionate'>,f1.308-f1,3,,1
wo<'affirm'>,f1.309-f1,3,,1
wo<'affirmative'>,f1.310-f1,3,,1
wo<'afford'>,f1.311-f1,3,,1
wo<'affront'>,f1.312-f1,3,,1
wo<'afraid'>,f1.313-f1,3,,1
wo<'afresh'>,f1.314-f1,3,,1
wo<'after'>,f1.315-f1,3,,1
wo<'after all'>,f1.316-f1,3,,1
wo<'afternoon'>,f1.317-f1,3,,1
wo<'afterwards'>,f1.318-f1,3,,1
wo<'again'>,f1.319-f1,3,,1
wo<'against'>,f1.320-f1,3,,1
wo<'age'>,f1.321-f1,3,,1
wo<'aged'>,f1.322-f1,3,,1
wo<'agency'>,f1.323-f1,3,,1
wo<'agenda'>,f1.324-f1,3,,1
wo<'agent'>,f1.325-f1,3,,1
wo<'aggravate'>,f1.326-f1,3,,1
wo<'aggression'>,f1.327-f1,3,,1
wo<'agitate'>,f1.328-f1,3,,1
wo<'agitator'>,f1.329-f1,3,,1
wo<'ago'>,f1.330-f1,3,,1
wo<'agony'>,f1.331-f1,3,,1
wo<'agree'>,f1.332-f1,3,,1
wo<'agreeable'>,f1.333-f1,3,,1
wo<'agreement'>,f1.334-f1,3,,1
wo<'agricultural'>,f1.335-f1,3,,1
wo<'agriculture'>,f1.336-f1,3,,1
wo<'ahead'>,f1.337-f1,3,,1
wo<'aid'>,f1.338-f1,3,,1
wo<'aim'>,f1.339-f1,3,,1
wo<'air'>,f1.340-f1,3,,1
wo<'air mail'>,f1.341-f1,3,,1
wo<'aircraft'>,f1.342-f1,3,,1
wo<'airman'>,f1.343-f1,3,,1
wo<'alarm'>,f1.344-f1,3,,1
wo<'alarm-clock'>,f1.345-f1,3,,1
wo<'alas'>,f1.346-f1,3,,1
wo<'alcohol'>,f1.347-f1,3,,1
wo<'alert'>,f1.348-f1,3,,1
wo<'algebra'>,f1.349-f1,3,,1
wo<'alien'>,f1.350-f1,3,,1
wo<'alight'>,f1.351-f1,3,,1
wo<'alike'>,f1.352-f1,3,,1
wo<'alive'>,f1.353-f1,3,,1
wo<'alive with'>,f1.354-f1,3,,1
wo<'all'>,f1.355-f1,3,,1
wo<'all children are alike'>,f1.356-f1,3,,1
wo<'all over the world'>,f1.357-f1,3,,1
wo<'all right'>,f1.358-f1,3,,1
wo<'allege'>,f1.359-f1,3,,1
wo<'allegiance'>,f1.360-f1,3,,1
wo<'alley'>,f1.361-f1,3,,1
wo<'alliance'>,f1.362-f1,3,,1
wo<'allied'>,f1.363-f1,3,,1
wo<'allot'>,f1.364-f1,3,,1
wo<'allotment'>,f1.365-f1,3,,1
wo<'allow'>,f1.366-f1,3,,1
wo<'allowance'>,f1.367-f1,3,,1
wo<'alloy'>,f1.368-f1,3,,1
wo<'allude'>,f1.369-f1,3,,1
wo<'allude to'>,f1.370-f1,3,,1
wo<'allusion'>,f1.371-f1,3,,1
wo<'ally'>,f1.372-f1,3,,1
wo<'almond'>,f1.373-f1,3,,1
wo<'almost'>,f1.374-f1,3,,1
wo<'alone'>,f1.375-f1,3,,1
wo<'along'>,f1.376-f1,3,,1
wo<'alongside'>,f1.377-f1,3,,1
wo<'aloud'>,f1.378-f1,3,,1
wo<'alphabet'>,f1.379-f1,3,,1
wo<'alpine'>,f1.380-f1,3,,1
wo<'already'>,f1.381-f1,3,,1
wo<'also'>,f1.382-f1,3,,1
wo<'alter'>,f1.383-f1,3,,1
wo<'alteration'>,f1.384-f1,3,,1
wo<'alternate'>,f1.385-f1,3,,1
wo<'alternative'>,f1.386-f1,3,,1
wo<'although'>,f1.387-f1,3,,1
wo<'altitude'>,f1.388-f1,3,,1
wo<'altogether'>,f1.389-f1,3,,1
wo<'aluminium'>,f1.390-f1,3,,1
wo<'always'>,f1.391-f1,3,,1
wo<'amalgamated'>,f1.392-f1,3,,1
wo<'amateur'>,f1.393-f1,3,,1
wo<'amaze'>,f1.394-f1,3,,1
wo<'amazement'>,f1.395-f1,3,,1
wo<'ambassador'>,f1.396-f1,3,,1
wo<'amber'>,f1.397-f1,3,,1
wo<'ambiguous'>,f1.398-f1,3,,1
wo<'ambition'>,f1.399-f1,3,,1
wo<'ambitious'>,f1.400-f1,3,,1
wo<'ambulance'>,f1.401-f1,3,,1
wo<'ambush'>,f1.402-f1,3,,1
wo<'amends'>,f1.403-f1,3,,1
wo<'amiable'>,f1.404-f1,3,,1
wo<'amicable'>,f1.405-f1,3,,1
wo<'amid'>,f1.406-f1,3,,1
wo<'amidst'>,f1.407-f1,3,,1
wo<'ammonia'>,f1.408-f1,3,,1
wo<'ammunition'>,f1.409-f1,3,,1
wo<'amnesty'>,f1.410-f1,3,,1
wo<'among'>,f1.411-f1,3,,1
wo<'amongst'>,f1.412-f1,3,,1
wo<'amount'>,f1.413-f1,3,,1
wo<'ample'>,f1.414-f1,3,,1
wo<'amputate'>,f1.415-f1,3,,1
wo<'amuse'>,f1.416-f1,3,,1
wo<'amusement'>,f1.417-f1,3,,1
wo<'amusing'>,f1.418-f1,3,,1
wo<'an air of importance'>,f1.419-f1,3,,1
wo<'anaesthetic'>,f1.420-f1,3,,1
wo<'analyse'>,f1.421-f1,3,,1
wo<'analysis'>,f1.422-f1,3,,1
wo<'anarchy'>,f1.423-f1,3,,1
wo<'anatomy'>,f1.424-f1,3,,1
wo<'ancestor'>,f1.425-f1,3,,1
wo<'anchor'>,f1.426-f1,3,,1
wo<'ancient'>,f1.427-f1,3,,1
wo<'and'>,f1.428-f1,3,,1
wo<'angel'>,f1.429-f1,3,,1
wo<'anger'>,f1.430-f1,3,,1
wo<'angle'>,f1.431-f1,3,,1
wo<'angry'>,f1.432-f1,3,,1
wo<'anguish'>,f1.433-f1,3,,1
wo<'animal'>,f1.434-f1,3,,1
wo<'animated'>,f1.435-f1,3,,1
wo<'animosity'>,f1.436-f1,3,,1
wo<'ankle'>,f1.437-f1,3,,1
wo<'annex'>,f1.438-f1,3,,1
wo<'annexation'>,f1.439-f1,3,,1
wo<'annihilate'>,f1.440-f1,3,,1
wo<'annihilation'>,f1.441-f1,3,,1
wo<'anniversary'>,f1.442-f1,3,,1
wo<'announce'>,f1.443-f1,3,,1
wo<'announcement'>,f1.444-f1,3,,1
wo<'announcer'>,f1.445-f1,3,,1
wo<'annoy'>,f1.446-f1,3,,1
wo<'annual'>,f1.447-f1,3,,1
wo<'annul'>,f1.448-f1,3,,1
wo<'another'>,f1.449-f1,3,,1
wo<'answer'>,f1.450-f1,3,,1
wo<'ant'>,f1.451-f1,3,,1
wo<'antarctic'>,f1.452-f1,3,,1
wo<'anthem'>,f1.453-f1,3,,1
wo<'anti-aircraft'>,f1.454-f1,3,,1
wo<'anti-fascist'>,f1.455-f1,3,,1
wo<'anticipate'>,f1.456-f1,3,,1
wo<'anticipation'>,f1.457-f1,3,,1
wo<'antidote'>,f1.458-f1,3,,1
wo<'antipathy'>,f1.459-f1,3,,1
wo<'antique'>,f1.460-f1,3,,1
wo<'antiquity'>,f1.461-f1,3,,1
wo<'anvil'>,f1.462-f1,3,,1
wo<'anxiety'>,f1.463-f1,3,,1
wo<'anxious'>,f1.464-f1,3,,1
wo<'any'>,f1.465-f1,3,,1
wo<'any comments'>,f1.466-f1,3,,1
wo<'anybody'>,f1.467-f1,3,,1
wo<'anyhow'>,f1.468-f1,3,,1
wo<'anyone'>,f1.469-f1,3,,1
wo<'anything'>,f1.470-f1,3,,1
wo<'anyway'>,f1.471-f1,3,,1
wo<'anywhere'>,f1.472-f1,3,,1
wo<'apart'>,f1.473-f1,3,,1
wo<'apart from'>,f1.474-f1,3,,1
wo<'apartament'>,f1.475-f1,3,,1
wo<'ape'>,f1.476-f1,3,,1
wo<'apiece'>,f1.477-f1,3,,1
wo<'apologize'>,f1.478-f1,3,,1
wo<'apology'>,f1.479-f1,3,,1
wo<'appal'>,f1.480-f1,3,,1
wo<'apparatus'>,f1.481-f1,3,,1
wo<'apparently'>,f1.482-f1,3,,1
wo<'appeal'>,f1.483-f1,3,,1
wo<'appear'>,f1.484-f1,3,,1
wo<'appearance'>,f1.485-f1,3,,1
wo<'appease'>,f1.486-f1,3,,1
wo<'appendix'>,f1.487-f1,3,,1
wo<'appetite'>,f1.488-f1,3,,1
wo<'applaud'>,f1.489-f1,3,,1
wo<'applause'>,f1.490-f1,3,,1
wo<'apple'>,f1.491-f1,3,,1
wo<'apple-tree'>,f1.492-f1,3,,1
wo<'appliance'>,f1.493-f1,3,,1
wo<'applicant'>,f1.494-f1,3,,1
wo<'application'>,f1.495-f1,3,,1
wo<'applied'>,f1.496-f1,3,,1
wo<'apply'>,f1.497-f1,3,,1
wo<'apply for'>,f1.498-f1,3,,1
wo<'apply to'>,f1.499-f1,3,,1
wo<'appoint'>,f1.500-f1,3,,1
wo<'appointment'>,f1.501-f1,3,,1
wo<'appraise'>,f1.502-f1,3,,1
wo<'appreciable'>,f1.503-f1,3,,1
wo<'appreciate'>,f1.504-f1,3,,1
wo<'apprentice'>,f1.505-f1,3,,1
wo<'apprenticeship'>,f1.506-f1,3,,1
wo<'approach'>,f1.507-f1,3,,1
wo<'approbation'>,f1.508-f1,3,,1
wo<'appropriate'>,f1.509-f1,3,,1
wo<'approval'>,f1.510-f1,3,,1
wo<'approve'>,f1.511-f1,3,,1
wo<'approximate'>,f1.512-f1,3,,1
wo<'apricot'>,f1.513-f1,3,,1
wo<'apron'>,f1.514-f1,3,,1
wo<'apt'>,f1.515-f1,3,,1
wo<'arable'>,f1.516-f1,3,,1
wo<'arbitrary'>,f1.517-f1,3,,1
wo<'arbour'>,f1.518-f1,3,,1
wo<'arc'>,f1.519-f1,3,,1
wo<'arch'>,f1.520-f1,3,,1
wo<'archaic'>,f1.521-f1,3,,1
wo<'archbishop'>,f1.522-f1,3,,1
wo<'architect'>,f1.523-f1,3,,1
wo<'architecture'>,f1.524-f1,3,,1
wo<'arctic'>,f1.525-f1,3,,1
wo<'ardent'>,f1.526-f1,3,,1
wo<'ardour'>,f1.527-f1,3,,1
wo<'arduous'>,f1.528-f1,3,,1
wo<'are'>,f1.529-f1,3,,1
wo<'are any tickets available'>,f1.530-f1,3,,1
wo<'area'>,f1.531-f1,3,,1
wo<'arena'>,f1.532-f1,3,,1
wo<'argue'>,f1.533-f1,3,,1
wo<'argument'>,f1.534-f1,3,,1
wo<'arid'>,f1.535-f1,3,,1
wo<'arise'>,f1.536-f1,3,,1
wo<'arithmetic'>,f1.537-f1,3,,1
wo<'arm'>,f1.538-f1,3,,1
wo<'arm-chair'>,f1.539-f1,3,,1
wo<'armament'>,f1.540-f1,3,,1
wo<'armed forces'>,f1.541-f1,3,,1
wo<'armistice'>,f1.542-f1,3,,1
wo<'armour'>,f1.543-f1,3,,1
wo<'armour-clad'>,f1.544-f1,3,,1
wo<'armoury'>,f1.545-f1,3,,1
wo<'arms'>,f1.546-f1,3,,1
wo<'army'>,f1.547-f1,3,,1
wo<'arose'>,f1.548-f1,3,,3
wo<'around'>,f1.549-f1,3,,1
wo<'arouse'>,f1.550-f1,3,,1
wo<'arrange'>,f1.551-f1,3,,1
wo<'arrangement'>,f1.552-f1,3,,1
wo<'arrest'>,f1.553-f1,3,,1
wo<'arrival'>,f1.554-f1,3,,1
wo<'arrive'>,f1.555-f1,3,,1
wo<'arrogance'>,f1.556-f1,3,,1
wo<'arrow'>,f1.557-f1,3,,1
wo<'arsenic'>,f1.558-f1,3,,1
wo<'art'>,f1.559-f1,3,,1
wo<'art-gallery'>,f1.560-f1,3,,1
wo<'article'>,f1.561-f1,3,,1
wo<'artificial'>,f1.562-f1,3,,1
wo<'artificial limb'>,f1.563-f1,3,,1
wo<'artificial teeth'>,f1.564-f1,3,,1
wo<'artillery'>,f1.565-f1,3,,1
wo<'artist'>,f1.566-f1,3,,1
wo<'artistic'>,f1.567-f1,3,,1
wo<'as'>,f1.568-f1,3,,1
wo<'as far as'>,f1.569-f1,3,,1
wo<'as followed'>,f1.570-f1,3,,1
wo<'as for me'>,f1.571-f1,3,,1
wo<'as if'>,f1.572-f1,3,,1
wo<'as soon as'>,f1.573-f1,3,,1
wo<'as to'>,f1.574-f1,3,,1
wo<'as well'>,f1.575-f1,3,,1
wo<'as well as'>,f1.576-f1,3,,1
wo<'ascent'>,f1.577-f1,3,,1
wo<'ascertain'>,f1.578-f1,3,,1
wo<'ash'>,f1.579-f1,3,,1
wo<'ashamed'>,f1.580-f1,3,,1
wo<'ashore'>,f1.581-f1,3,,1
wo<'aside'>,f1.582-f1,3,,1
wo<'ask'>,f1.583-f1,3,,1
wo<'ask after'>,f1.584-f1,3,,1
wo<'ask question'>,f1.585-f1,3,,1
wo<'asp'>,f1.586-f1,3,,1
wo<'aspect'>,f1.587-f1,3,,1
wo<'aspiration'>,f1.588-f1,3,,1
wo<'aspire'>,f1.589-f1,3,,1
wo<'ass'>,f1.590-f1,3,,1
wo<'assassin'>,f1.591-f1,3,,1
wo<'assassination'>,f1.592-f1,3,,1
wo<'assault'>,f1.593-f1,3,,1
wo<'assemble'>,f1.594-f1,3,,1
wo<'assembly'>,f1.595-f1,3,,1
wo<'assent'>,f1.596-f1,3,,1
wo<'assert'>,f1.597-f1,3,,1
wo<'assert oneself'>,f1.598-f1,3,,1
wo<'assert sovereignty'>,f1.599-f1,3,,1
wo<'assign'>,f1.600-f1,3,,1
wo<'assignment'>,f1.601-f1,3,,1
wo<'assimilate'>,f1.602-f1,3,,1
wo<'assist'>,f1.603-f1,3,,1
wo<'assistance'>,f1.604-f1,3,,1
wo<'assistance pack'>,f1.605-f1,3,,1
wo<'assistant'>,f1.606-f1,3,,1
wo<'associate'>,f1.607-f1,3,,1
wo<'association'>,f1.608-f1,3,,1
wo<'assortment'>,f1.609-f1,3,,1
wo<'assume'>,f1.610-f1,3,,1
wo<'assure'>,f1.611-f1,3,,1
wo<'astonish'>,f1.612-f1,3,,1
wo<'astonishment'>,f1.613-f1,3,,1
wo<'astronautics'>,f1.614-f1,3,,1
wo<'astronomy'>,f1.615-f1,3,,1
wo<'asylum'>,f1.616-f1,3,,1
wo<'at'>,f1.617-f1,3,,1
wo<'at all'>,f1.618-f1,3,,1
wo<'at home'>,f1.619-f1,3,,1
wo<'at last'>,f1.620-f1,3,,1
wo<'at the head of'>,f1.621-f1,3,,1
wo<'ate'>,f1.622-f1,3,,3
wo<'atheist'>,f1.623-f1,3,,1
wo<'athlete'>,f1.624-f1,3,,1
wo<'athletics'>,f1.625-f1,3,,1
wo<'atlas'>,f1.626-f1,3,,1
wo<'atmosphere'>,f1.627-f1,3,,1
wo<'atom'>,f1.628-f1,3,,1
wo<'atom bomb'>,f1.629-f1,3,,1
wo<'atomic'>,f1.630-f1,3,,1
wo<'atrocious'>,f1.631-f1,3,,1
wo<'atrocity'>,f1.632-f1,3,,1
wo<'attach'>,f1.633-f1,3,,1
wo<'attachment'>,f1.634-f1,3,,1
wo<'attack'>,f1.635-f1,3,,1
wo<'attain'>,f1.636-f1,3,,1
wo<'attempt'>,f1.637-f1,3,,1
wo<'attend'>,f1.638-f1,3,,1
wo<'attendance'>,f1.639-f1,3,,1
wo<'attendant'>,f1.640-f1,3,,1
wo<'attention'>,f1.641-f1,3,,1
wo<'attentive'>,f1.642-f1,3,,1
wo<'attic'>,f1.643-f1,3,,1
wo<'attitude'>,f1.644-f1,3,,1
wo<'attorney'>,f1.645-f1,3,,1
wo<'attract'>,f1.646-f1,3,,1
wo<'attraction'>,f1.647-f1,3,,1
wo<'attractive'>,f1.648-f1,3,,1
wo<'attribute'>,f1.649-f1,3,,1
wo<'auction'>,f1.650-f1,3,,1
wo<'audacious'>,f1.651-f1,3,,1
wo<'audacity'>,f1.652-f1,3,,1
wo<'audible'>,f1.653-f1,3,,1
wo<'audience'>,f1.654-f1,3,,1
wo<'aunt'>,f1.655-f1,3,,1
wo<'austere'>,f1.656-f1,3,,1
wo<'authentic'>,f1.657-f1,3,,1
wo<'author'>,f1.658-f1,3,,1
wo<'authoritative'>,f1.659-f1,3,,1
wo<'authority'>,f1.660-f1,3,,1
wo<'authorize'>,f1.661-f1,3,,1
wo<'autobiographic'>,f1.662-f1,3,,1
wo<'autobiographical'>,f1.663-f1,3,,1
wo<'autobiography'>,f1.664-f1,3,,1
wo<'automatic'>,f1.665-f1,3,,1
wo<'automobile'>,f1.666-f1,3,,1
wo<'autonomous'>,f1.667-f1,3,,1
wo<'autumn'>,f1.668-f1,3,,1
wo<'auxiliary'>,f1.669-f1,3,,1
wo<'available'>,f1.670-f1,3,,1
wo<'avalanche'>,f1.671-f1,3,,1
wo<'avarice'>,f1.672-f1,3,,1
wo<'avenge'>,f1.673-f1,3,,1
wo<'avenue'>,f1.674-f1,3,,1
wo<'average'>,f1.675-f1,3,,1
wo<'aversion'>,f1.676-f1,3,,1
wo<'avert'>,f1.677-f1,3,,1
wo<'aviation'>,f1.678-f1,3,,1
wo<'avoid'>,f1.679-f1,3,,1
wo<'awake'>,f1.680-f1,3,,1
wo<'award'>,f1.681-f1,3,,1
wo<'away'>,f1.682-f1,3,,1
wo<'awe'>,f1.683-f1,3,,1
wo<'awful'>,f1.684-f1,3,,1
wo<'awfully'>,f1.685-f1,3,,1
wo<'awkward'>,f1.686-f1,3,,1
wo<'axe'>,f1.687-f1,3,,1
wo<'axis'>,f1.688-f1,3,,1
wo<'azure'>,f1.689-f1,3,,1
wo<'baby'>,f1.690-f1,3,,1
wo<'babyish'>,f1.691-f1,3,,1
wo<'back'>,f1.692-f1,3,,1
wo<'backbone'>,f1.693-f1,3,,1
wo<'background'>,f1.694-f1,3,,1
wo<'backwards'>,f1.695-f1,3,,1
wo<'bacon'>,f1.696-f1,3,,1
wo<'bad'>,f1.697-f1,3,,1
wo<'bad language'>,f1.698-f1,3,,1
wo<'bad record'>,f1.699-f1,3,,1
wo<'badge'>,f1.700-f1,3,,1
wo<'bag'>,f1.701-f1,3,,1
wo<'baggage'>,f1.702-f1,3,,1
wo<'bail'>,f1.703-f1,3,,1
wo<'bait'>,f1.704-f1,3,,1
wo<'bake'>,f1.705-f1,3,,1
wo<'baker'>,f1.706-f1,3,,1
wo<'bakery'>,f1.707-f1,3,,1
wo<'balcony'>,f1.708-f1,3,,1
wo<'ball'>,f1.709-f1,3,,1
wo<'ballad'>,f1.710-f1,3,,1
wo<'ballon'>,f1.711-f1,3,,1
wo<'bank'>,f1.712-f1,3,,1
wo<'barley'>,f1.713-f1,3,,1
wo<'barn'>,f1.714-f1,3,,1
wo<'barrack'>,f1.715-f1,3,,1
wo<'barrage'>,f1.716-f1,3,,1
wo<'barrel'>,f1.717-f1,3,,1
wo<'barren'>,f1.718-f1,3,,1
wo<'barrier'>,f1.719-f1,3,,1
wo<'barrister'>,f1.720-f1,3,,1
wo<'base'>,f1.721-f1,3,,1
wo<'baseball'>,f1.722-f1,3,,1
wo<'basement'>,f1.723-f1,3,,1
wo<'basic'>,f1.724-f1,3,,1
wo<'basin'>,f1.725-f1,3,,1
wo<'basis'>,f1.726-f1,3,,1
wo<'basket'>,f1.727-f1,3,,1
wo<'basket-ball'>,f1.728-f1,3,,1
wo<'basket-ball player'>,f1.729-f1,3,,1
wo<'bathe'>,f1.730-f1,3,,1
wo<'bathroom'>,f1.731-f1,3,,1
wo<'battle'>,f1.732-f1,3,,1
wo<'battleship'>,f1.733-f1,3,,1
wo<'bay'>,f1.734-f1,3,,1
wo<'bayonet'>,f1.735-f1,3,,1
wo<'be'>,f1.736-f1,3,,1
wo<'be able to'>,f1.737-f1,3,,1
wo<'be absent'>,f1.738-f1,3,,1
wo<'be accustomed'>,f1.739-f1,3,,1
wo<'be afraid'>,f1.740-f1,3,,1
wo<'be afraid of'>,f1.741-f1,3,,1
wo<'be alive to'>,f1.742-f1,3,,1
wo<'be angry'>,f1.743-f1,3,,1
wo<'be ashamed'>,f1.744-f1,3,,1
wo<'be asleep'>,f1.745-f1,3,,1
wo<'be averse to'>,f1.746-f1,3,,1
wo<'be awake'>,f1.747-f1,3,,1
wo<'be aware of'>,f1.748-f1,3,,1
wo<'be born'>,f1.749-f1,3,,1
wo<'be certain'>,f1.750-f1,3,,1
wo<'be fond of'>,f1.751-f1,3,,1
wo<'be in'>,f1.752-f1,3,,1
wo<'be in charge of'>,f1.753-f1,3,,1
wo<'be late'>,f1.754-f1,3,,1
wo<'be on duty'>,f1.755-f1,3,,1
wo<'be on the watch'>,f1.756-f1,3,,1
wo<'be out'>,f1.757-f1,3,,1
wo<'be over'>,f1.758-f1,3,,1
wo<'be proud of'>,f1.759-f1,3,,1
wo<'be quick'>,f1.760-f1,3,,1
wo<'be quits'>,f1.761-f1,3,,1
wo<'be ready'>,f1.762-f1,3,,1
wo<'be reluctant'>,f1.763-f1,3,,1
wo<'be sure'>,f1.764-f1,3,,1
wo<'be well'>,f1.765-f1,3,,1
wo<'bean'>,f1.766-f1,3,,1
wo<'beat'>,f1.767-f1,3,,1
wo<'beautiful'>,f1.768-f1,3,,1
wo<'became'>,f1.769-f1,3,,3
wo<'because'>,f1.770-f1,3,,1
wo<'becauseof'>,f1.771-f1,3,,1
wo<'beckon'>,f1.772-f1,3,,1
wo<'become'>,f1.773-f1,3,,1
wo<'becycle'>,f1.774-f1,3,,1
wo<'bed'>,f1.775-f1,3,,1
wo<'bee'>,f1.776-f1,3,,1
wo<'beech'>,f1.777-f1,3,,1
wo<'beef'>,f1.778-f1,3,,1
wo<'beehive'>,f1.779-f1,3,,1
wo<'beer'>,f1.780-f1,3,,1
wo<'beet'>,f1.781-f1,3,,1
wo<'beetle'>,f1.782-f1,3,,1
wo<'beetroot'>,f1.783-f1,3,,1
wo<'before'>,f1.784-f1,3,,1
wo<'beg'>,f1.785-f1,3,,1
wo<'began'>,f1.786-f1,3,,3
wo<'beggar'>,f1.787-f1,3,,1
wo<'begin'>,f1.788-f1,3,,1
wo<'behind'>,f1.789-f1,3,,1
wo<'belief'>,f1.790-f1,3,,1
wo<'believe'>,f1.791-f1,3,,1
wo<'bell'>,f1.792-f1,3,,1
wo<'bellow'>,f1.793-f1,3,,1
wo<'belong'>,f1.794-f1,3,,1
wo<'below'>,f1.795-f1,3,,1
wo<'belt'>,f1.796-f1,3,,1
wo<'bench'>,f1.797-f1,3,,1
wo<'berry'>,f1.798-f1,3,,1
wo<'besides'>,f1.799-f1,3,,1
wo<'best'>,f1.800-f1,3,,1
wo<'better'>,f1.801-f1,3,,1
wo<'between'>,f1.802-f1,3,,1
wo<'beware'>,f1.803-f1,3,,1
wo<'bewildered'>,f1.804-f1,3,,1
wo<'big'>,f1.805-f1,3,,1
wo<'biography'>,f1.806-f1,3,,1
wo<'biology'>,f1.807-f1,3,,1
wo<'bird'>,f1.808-f1,3,,1
wo<'birthday'>,f1.809-f1,3,,1
wo<'black'>,f1.810-f1,3,,1
wo<'black-out'>,f1.811-f1,3,,1
wo<'blackbird'>,f1.812-f1,3,,1
wo<'blackboard'>,f1.813-f1,3,,1
wo<'blacken'>,f1.814-f1,3,,1
wo<'blacksmith'>,f1.815-f1,3,,1
wo<'blade'>,f1.816-f1,3,,1
wo<'blame'>,f1.817-f1,3,,1
wo<'blank'>,f1.818-f1,3,,1
wo<'blanket'>,f1.819-f1,3,,1
wo<'blast'>,f1.820-f1,3,,1
wo<'blast-furnace'>,f1.821-f1,3,,1
wo<'blaze'>,f1.822-f1,3,,1
wo<'bleach'>,f1.823-f1,3,,1
wo<'bleak'>,f1.824-f1,3,,1
wo<'blew'>,f1.825-f1,3,,3
wo<'bloom'>,f1.826-f1,3,,1
wo<'blouse'>,f1.827-f1,3,,1
wo<'blow'>,f1.828-f1,3,,1
wo<'blue'>,f1.829-f1,3,,1
wo<'blue-eyed'>,f1.830-f1,3,,1
wo<'blunder'>,f1.831-f1,3,,1
wo<'blunt'>,f1.832-f1,3,,1
wo<'blush'>,f1.833-f1,3,,1
wo<'board'>,f1.834-f1,3,,1
wo<'board and lodging'>,f1.835-f1,3,,1
wo<'boarding-house'>,f1.836-f1,3,,1
wo<'boarding-school'>,f1.837-f1,3,,1
wo<'boast'>,f1.838-f1,3,,1
wo<'boat'>,f1.839-f1,3,,1
wo<'body'>,f1.840-f1,3,,1
wo<'bog'>,f1.841-f1,3,,1
wo<'bomb'>,f1.842-f1,3,,1
wo<'book'>,f1.843-f1,3,,1
wo<'bookcase'>,f1.844-f1,3,,1
wo<'booking-office'>,f1.845-f1,3,,1
wo<'boom'>,f1.846-f1,3,,1
wo<'boot'>,f1.847-f1,3,,1
wo<'booth'>,f1.848-f1,3,,1
wo<'booty'>,f1.849-f1,3,,1
wo<'border'>,f1.850-f1,3,,1
wo<'bore'>,f1.851-f1,3,,1
wo<'born'>,f1.852-f1,3,,1
wo<'botany'>,f1.853-f1,3,,1
wo<'both'>,f1.854-f1,3,,1
wo<'bottle'>,f1.855-f1,3,,1
wo<'bottom'>,f1.856-f1,3,,1
wo<'bought'>,f1.857-f1,3,,1
wo<'box'>,f1.858-f1,3,,1
wo<'box on the ear'>,f1.859-f1,3,,1
wo<'boxer'>,f1.860-f1,3,,1
wo<'boxing'>,f1.861-f1,3,,1
wo<'boy'>,f1.862-f1,3,,1
wo<'braid'>,f1.863-f1,3,,1
wo<'brake'>,f1.864-f1,3,,1
wo<'branch'>,f1.865-f1,3,,1
wo<'brave'>,f1.866-f1,3,,1
wo<'bravery'>,f1.867-f1,3,,1
wo<'bread'>,f1.868-f1,3,,1
wo<'break'>,f1.869-f1,3,,1
wo<'breakfast'>,f1.870-f1,3,,1
wo<'bride'>,f1.871-f1,3,,1
wo<'bridegroom'>,f1.872-f1,3,,1
wo<'bridge'>,f1.873-f1,3,,1
wo<'bridge of one',39,'s nose'>,f1.874-f1,3,,1
wo<'bridle'>,f1.875-f1,3,,1
wo<'brief'>,f1.876-f1,3,,1
wo<'brief case'>,f1.877-f1,3,,1
wo<'brigade'>,f1.878-f1,3,,1
wo<'bright'>,f1.879-f1,3,,1
wo<'brilliant'>,f1.880-f1,3,,1
wo<'brim'>,f1.881-f1,3,,1
wo<'bring'>,f1.882-f1,3,,1
wo<'bring about'>,f1.883-f1,3,,1
wo<'bring up'>,f1.884-f1,3,,1
wo<'brink'>,f1.885-f1,3,,1
wo<'brisk'>,f1.886-f1,3,,1
wo<'bristle'>,f1.887-f1,3,,1
wo<'brittle'>,f1.888-f1,3,,1
wo<'broad'>,f1.889-f1,3,,1
wo<'broke'>,f1.890-f1,3,,3
wo<'brooch'>,f1.891-f1,3,,1
wo<'brood'>,f1.892-f1,3,,1
wo<'brook'>,f1.893-f1,3,,1
wo<'broth'>,f1.894-f1,3,,1
wo<'brother'>,f1.895-f1,3,,1
wo<'brought'>,f1.896-f1,3,,1
wo<'brown'>,f1.897-f1,3,,1
wo<'brush'>,f1.898-f1,3,,1
wo<'brutal'>,f1.899-f1,3,,1
wo<'brute'>,f1.900-f1,3,,1
wo<'bubble'>,f1.901-f1,3,,1
wo<'bucket'>,f1.902-f1,3,,1
wo<'budget'>,f1.903-f1,3,,1
wo<'bug'>,f1.904-f1,3,,1
wo<'build'>,f1.905-f1,3,,1
wo<'builder'>,f1.906-f1,3,,1
wo<'building'>,f1.907-f1,3,,1
wo<'built'>,f1.908-f1,3,,1
wo<'buoy'>,f1.909-f1,3,,1
wo<'burnt'>,f1.910-f1,3,,1
wo<'burst'>,f1.911-f1,3,,1
wo<'bus'>,f1.912-f1,3,,1
wo<'bust'>,f1.913-f1,3,,1
wo<'bustle'>,f1.914-f1,3,,1
wo<'but'>,f1.915-f1,3,,1
wo<'butter'>,f1.916-f1,3,,1
wo<'butterfly'>,f1.917-f1,3,,1
wo<'button'>,f1.918-f1,3,,1
wo<'buttonhole'>,f1.919-f1,3,,1
wo<'buy'>,f1.920-f1,3,,1
wo<'buzz'>,f1.921-f1,3,,1
wo<'buzzer'>,f1.922-f1,3,,1
wo<'by'>,f1.923-f1,3,,1
wo<'by accident'>,f1.924-f1,3,,1
wo<'by all means'>,f1.925-f1,3,,1
wo<'by bus'>,f1.926-f1,3,,1
wo<'by degrees'>,f1.927-f1,3,,1
wo<'by fits and stars'>,f1.928-f1,3,,1
wo<'by means of'>,f1.929-f1,3,,1
wo<'by no means'>,f1.930-f1,3,,1
wo<'by the agency of'>,f1.931-f1,3,,1
wo<'by through the agency of'>,f1.932-f1,3,,1
wo<'byword'>,f1.933-f1,3,,1
wo<'cab'>,f1.934-f1,3,,1
wo<'cabbage'>,f1.935-f1,3,,1
wo<'cabin'>,f1.936-f1,3,,1
wo<'cabinet'>,f1.937-f1,3,,1
wo<'cable'>,f1.938-f1,3,,1
wo<'cage'>,f1.939-f1,3,,1
wo<'cake'>,f1.940-f1,3,,1
wo<'cake of soap'>,f1.941-f1,3,,1
wo<'calamity'>,f1.942-f1,3,,1
wo<'calculate'>,f1.943-f1,3,,1
wo<'calculating-machine'>,f1.944-f1,3,,1
wo<'calendar'>,f1.945-f1,3,,1
wo<'calf'>,f1.946-f1,3,,1
wo<'call'>,f1.947-f1,3,,1
wo<'call for'>,f1.948-f1,3,,1
wo<'call on'>,f1.949-f1,3,,1
wo<'call up'>,f1.950-f1,3,,1
wo<'callous'>,f1.951-f1,3,,1
wo<'calm'>,f1.952-f1,3,,1
wo<'came'>,f1.953-f1,3,,3
wo<'camel'>,f1.954-f1,3,,1
wo<'camp'>,f1.955-f1,3,,1
wo<'can'>,f1.956-f1,3,,1
wo<'cancel'>,f1.957-f1,3,,1
wo<'cancer'>,f1.958-f1,3,,1
wo<'candid'>,f1.959-f1,3,,1
wo<'candle'>,f1.960-f1,3,,1
wo<'candle-stick'>,f1.961-f1,3,,1
wo<'cap'>,f1.962-f1,3,,1
wo<'capacity'>,f1.963-f1,3,,1
wo<'cape'>,f1.964-f1,3,,1
wo<'capital'>,f1.965-f1,3,,1
wo<'captain'>,f1.966-f1,3,,1
wo<'captive'>,f1.967-f1,3,,1
wo<'capture'>,f1.968-f1,3,,1
wo<'car'>,f1.969-f1,3,,1
wo<'carbon'>,f1.970-f1,3,,1
wo<'carbon-paper'>,f1.971-f1,3,,1
wo<'cardboard'>,f1.972-f1,3,,1
wo<'care'>,f1.973-f1,3,,1
wo<'carpenter'>,f1.974-f1,3,,1
wo<'carpet'>,f1.975-f1,3,,1
wo<'carry'>,f1.976-f1,3,,1
wo<'carry out'>,f1.977-f1,3,,1
wo<'cart'>,f1.978-f1,3,,1
wo<'cartoon'>,f1.979-f1,3,,1
wo<'case'>,f1.980-f1,3,,1
wo<'cash'>,f1.981-f1,3,,1
wo<'cash a cheque'>,f1.982-f1,3,,1
wo<'cash register'>,f1.983-f1,3,,1
wo<'cashier'>,f1.984-f1,3,,1
wo<'cast'>,f1.985-f1,3,,1
wo<'cast iron'>,f1.986-f1,3,,1
wo<'castle'>,f1.987-f1,3,,1
wo<'cat'>,f1.988-f1,3,,1
wo<'catch'>,f1.989-f1,3,,1
wo<'cattle'>,f1.990-f1,3,,1
wo<'cattle-farm'>,f1.991-f1,3,,1
wo<'caught'>,f1.992-f1,3,,1
wo<'cause'>,f1.993-f1,3,,1
wo<'cedar'>,f1.994-f1,3,,1
wo<'ceiling'>,f1.995-f1,3,,1
wo<'celebrate'>,f1.996-f1,3,,1
wo<'celebration'>,f1.997-f1,3,,1
wo<'cell'>,f1.998-f1,3,,1
wo<'cement sheath'>,f1.999-f1,3,,1
wo<'cemetery'>,f1.1000-f1,3,,1
wo<'central'>,f1.1001-f1,3,,1
wo<'centre'>,f1.1002-f1,3,,1
wo<'century'>,f1.1003-f1,3,,1
wo<'cereals'>,f1.1004-f1,3,,1
wo<'ceremony'>,f1.1005-f1,3,,1
wo<'certain'>,f1.1006-f1,3,,1
wo<'certainly'>,f1.1007-f1,3,,1
wo<'certainty'>,f1.1008-f1,3,,1
wo<'certificate'>,f1.1009-f1,3,,1
wo<'certitude'>,f1.1010-f1,3,,1
wo<'cessation'>,f1.1011-f1,3,,1
wo<'chain'>,f1.1012-f1,3,,1
wo<'chair'>,f1.1013-f1,3,,1
wo<'chalk'>,f1.1014-f1,3,,1
wo<'champion'>,f1.1015-f1,3,,1
wo<'championship'>,f1.1016-f1,3,,1
wo<'chance'>,f1.1017-f1,3,,1
wo<'change'>,f1.1018-f1,3,,1
wo<'charge'>,f1.1019-f1,3,,1
wo<'chart'>,f1.1020-f1,3,,1
wo<'charter'>,f1.1021-f1,3,,1
wo<'charwoman'>,f1.1022-f1,3,,1
wo<'cheek'>,f1.1023-f1,3,,1
wo<'cheese'>,f1.1024-f1,3,,1
wo<'chemical'>,f1.1025-f1,3,,1
wo<'chemise'>,f1.1026-f1,3,,1
wo<'chemist'>,f1.1027-f1,3,,1
wo<'chemistry'>,f1.1028-f1,3,,1
wo<'cherry'>,f1.1029-f1,3,,1
wo<'chess'>,f1.1030-f1,3,,1
wo<'chess player'>,f1.1031-f1,3,,1
wo<'chestnut tree'>,f1.1032-f1,3,,1
wo<'chicken'>,f1.1033-f1,3,,1
wo<'chief'>,f1.1034-f1,3,,1
wo<'chiefly'>,f1.1035-f1,3,,1
wo<'child'>,f1.1036-f1,3,,1
wo<'childhood'>,f1.1037-f1,3,,1
wo<'children'>,f1.1038-f1,3,,1
wo<'chime'>,f1.1039-f1,3,,1
wo<'chimney'>,f1.1040-f1,3,,1
wo<'chin'>,f1.1041-f1,3,,1
wo<'china'>,f1.1042-f1,3,,1
wo<'chip'>,f1.1043-f1,3,,1
wo<'chirp'>,f1.1044-f1,3,,1
wo<'chivalrous'>,f1.1045-f1,3,,1
wo<'choice'>,f1.1046-f1,3,,1
wo<'choir'>,f1.1047-f1,3,,1
wo<'choke'>,f1.1048-f1,3,,1
wo<'choose'>,f1.1049-f1,3,,1
wo<'chop'>,f1.1050-f1,3,,1
wo<'chose'>,f1.1051-f1,3,,3
wo<'church'>,f1.1052-f1,3,,1
wo<'cinder'>,f1.1053-f1,3,,1
wo<'cinema'>,f1.1054-f1,3,,1
wo<'cipher'>,f1.1055-f1,3,,1
wo<'circle'>,f1.1056-f1,3,,1
wo<'circus'>,f1.1057-f1,3,,1
wo<'cite'>,f1.1058-f1,3,,1
wo<'city'>,f1.1059-f1,3,,1
wo<'claim'>,f1.1060-f1,3,,1
wo<'clamour'>,f1.1061-f1,3,,1
wo<'clap'>,f1.1062-f1,3,,1
wo<'clash'>,f1.1063-f1,3,,1
wo<'clasp'>,f1.1064-f1,3,,1
wo<'class'>,f1.1065-f1,3,,1
wo<'classmate'>,f1.1066-f1,3,,1
wo<'classroom'>,f1.1067-f1,3,,1
wo<'clean'>,f1.1068-f1,3,,1
wo<'clench'>,f1.1069-f1,3,,1
wo<'clergy'>,f1.1070-f1,3,,1
wo<'clergyman'>,f1.1071-f1,3,,1
wo<'clerk'>,f1.1072-f1,3,,1
wo<'clever'>,f1.1073-f1,3,,1
wo<'client'>,f1.1074-f1,3,,1
wo<'cliff'>,f1.1075-f1,3,,1
wo<'climate'>,f1.1076-f1,3,,1
wo<'climatic'>,f1.1077-f1,3,,1
wo<'cloak-room'>,f1.1078-f1,3,,1
wo<'clock'>,f1.1079-f1,3,,1
wo<'close'>,f1.1080-f1,3,,1
wo<'closet'>,f1.1081-f1,3,,1
wo<'cloth'>,f1.1082-f1,3,,1
wo<'clothe'>,f1.1083-f1,3,,1
wo<'clothes'>,f1.1084-f1,3,,1
wo<'club'>,f1.1085-f1,3,,1
wo<'coal'>,f1.1086-f1,3,,1
wo<'coast'>,f1.1087-f1,3,,1
wo<'coat'>,f1.1088-f1,3,,1
wo<'cocoa'>,f1.1089-f1,3,,1
wo<'cocoon'>,f1.1090-f1,3,,1
wo<'cod'>,f1.1091-f1,3,,1
wo<'code'>,f1.1092-f1,3,,1
wo<'coffee'>,f1.1093-f1,3,,1
wo<'coffee-beans'>,f1.1094-f1,3,,1
wo<'coffee-pot'>,f1.1095-f1,3,,1
wo<'coffin'>,f1.1096-f1,3,,1
wo<'coil'>,f1.1097-f1,3,,1
wo<'coin'>,f1.1098-f1,3,,1
wo<'coincidence'>,f1.1099-f1,3,,1
wo<'coke'>,f1.1100-f1,3,,1
wo<'cold'>,f1.1101-f1,3,,1
wo<'collar'>,f1.1102-f1,3,,1
wo<'collect'>,f1.1103-f1,3,,1
wo<'collection'>,f1.1104-f1,3,,1
wo<'colour'>,f1.1105-f1,3,,1
wo<'column'>,f1.1106-f1,3,,1
wo<'comb'>,f1.1107-f1,3,,1
wo<'combat'>,f1.1108-f1,3,,1
wo<'combination'>,f1.1109-f1,3,,1
wo<'combine'>,f1.1110-f1,3,,1
wo<'combine-operator'>,f1.1111-f1,3,,1
wo<'combustion'>,f1.1112-f1,3,,1
wo<'come'>,f1.1113-f1,3,,1
wo<'come across'>,f1.1114-f1,3,,1
wo<'come back'>,f1.1115-f1,3,,1
wo<'come in time'>,f1.1116-f1,3,,1
wo<'comedy'>,f1.1117-f1,3,,1
wo<'comfort'>,f1.1118-f1,3,,1
wo<'comfortable'>,f1.1119-f1,3,,1
wo<'comic'>,f1.1120-f1,3,,1
wo<'coming'>,f1.1121-f1,3,,1
wo<'comma'>,f1.1122-f1,3,,1
wo<'command'>,f1.1123-f1,3,,1
wo<'commander'>,f1.1124-f1,3,,1
wo<'commend'>,f1.1125-f1,3,,1
wo<'comment'>,f1.1126-f1,3,,1
wo<'commentator'>,f1.1127-f1,3,,1
wo<'commerce'>,f1.1128-f1,3,,1
wo<'commercial'>,f1.1129-f1,3,,1
wo<'commission'>,f1.1130-f1,3,,1
wo<'commit'>,f1.1131-f1,3,,1
wo<'committee'>,f1.1132-f1,3,,1
wo<'commodity'>,f1.1133-f1,3,,1
wo<'common'>,f1.1134-f1,3,,1
wo<'commonplace'>,f1.1135-f1,3,,1
wo<'commotion'>,f1.1136-f1,3,,1
wo<'commune'>,f1.1137-f1,3,,1
wo<'communicate'>,f1.1138-f1,3,,1
wo<'communication'>,f1.1139-f1,3,,1
wo<'communism'>,f1.1140-f1,3,,1
wo<'communist'>,f1.1141-f1,3,,1
wo<'community'>,f1.1142-f1,3,,1
wo<'compact'>,f1.1143-f1,3,,1
wo<'companion'>,f1.1144-f1,3,,1
wo<'company'>,f1.1145-f1,3,,1
wo<'comparative'>,f1.1146-f1,3,,1
wo<'compare'>,f1.1147-f1,3,,1
wo<'comparison'>,f1.1148-f1,3,,1
wo<'compartment'>,f1.1149-f1,3,,1
wo<'compass'>,f1.1150-f1,3,,1
wo<'compassion'>,f1.1151-f1,3,,1
wo<'compatriot'>,f1.1152-f1,3,,1
wo<'compel'>,f1.1153-f1,3,,1
wo<'compensate'>,f1.1154-f1,3,,1
wo<'compete'>,f1.1155-f1,3,,1
wo<'competence'>,f1.1156-f1,3,,1
wo<'competent'>,f1.1157-f1,3,,1
wo<'competition'>,f1.1158-f1,3,,1
wo<'competitor'>,f1.1159-f1,3,,1
wo<'compile'>,f1.1160-f1,3,,1
wo<'complacent'>,f1.1161-f1,3,,1
wo<'complain'>,f1.1162-f1,3,,1
wo<'complaint'>,f1.1163-f1,3,,1
wo<'complement'>,f1.1164-f1,3,,1
wo<'complete'>,f1.1165-f1,3,,1
wo<'completely'>,f1.1166-f1,3,,1
wo<'complex'>,f1.1167-f1,3,,1
wo<'complexion'>,f1.1168-f1,3,,1
wo<'compliance'>,f1.1169-f1,3,,1
wo<'complicate'>,f1.1170-f1,3,,1
wo<'complicated'>,f1.1171-f1,3,,1
wo<'compliment'>,f1.1172-f1,3,,1
wo<'complimentary'>,f1.1173-f1,3,,1
wo<'complimentary copy'>,f1.1174-f1,3,,1
wo<'complimentary ticket'>,f1.1175-f1,3,,1
wo<'comply'>,f1.1176-f1,3,,1
wo<'compose'>,f1.1177-f1,3,,1
wo<'compose oneself'>,f1.1178-f1,3,,1
wo<'composed'>,f1.1179-f1,3,,1
wo<'composer'>,f1.1180-f1,3,,1
wo<'composition'>,f1.1181-f1,3,,1
wo<'compositor'>,f1.1182-f1,3,,1
wo<'composure'>,f1.1183-f1,3,,1
wo<'compound'>,f1.1184-f1,3,,1
wo<'comprehensive'>,f1.1185-f1,3,,1
wo<'compress'>,f1.1186-f1,3,,1
wo<'comprise'>,f1.1187-f1,3,,1
wo<'compromise'>,f1.1188-f1,3,,1
wo<'compulsory'>,f1.1189-f1,3,,1
wo<'computer'>,f1.1190-f1,3,,1
wo<'comrade'>,f1.1191-f1,3,,1
wo<'concave'>,f1.1192-f1,3,,1
wo<'conceal'>,f1.1193-f1,3,,1
wo<'concede'>,f1.1194-f1,3,,1
wo<'concert'>,f1.1195-f1,3,,1
wo<'condition'>,f1.1196-f1,3,,1
wo<'conduct'>,f1.1197-f1,3,,1
wo<'conductor'>,f1.1198-f1,3,,1
wo<'cone'>,f1.1199-f1,3,,1
wo<'conference'>,f1.1200-f1,3,,1
wo<'confess'>,f1.1201-f1,3,,1
wo<'confession'>,f1.1202-f1,3,,1
wo<'confide'>,f1.1203-f1,3,,1
wo<'confidence'>,f1.1204-f1,3,,1
wo<'confident'>,f1.1205-f1,3,,1
wo<'confidential'>,f1.1206-f1,3,,1
wo<'confine'>,f1.1207-f1,3,,1
wo<'confinement'>,f1.1208-f1,3,,1
wo<'confirm'>,f1.1209-f1,3,,1
wo<'confirmation'>,f1.1210-f1,3,,1
wo<'conflict'>,f1.1211-f1,3,,1
wo<'conform'>,f1.1212-f1,3,,1
wo<'congratulate'>,f1.1213-f1,3,,1
wo<'connect'>,f1.1214-f1,3,,1
wo<'consist of'>,f1.1215-f1,3,,1
wo<'consistent'>,f1.1216-f1,3,,1
wo<'consolation'>,f1.1217-f1,3,,1
wo<'console'>,f1.1218-f1,3,,1
wo<'consolidate'>,f1.1219-f1,3,,1
wo<'consonant'>,f1.1220-f1,3,,1
wo<'conspicuous'>,f1.1221-f1,3,,1
wo<'conspiracy'>,f1.1222-f1,3,,1
wo<'conspire'>,f1.1223-f1,3,,1
wo<'constancy'>,f1.1224-f1,3,,1
wo<'constant'>,f1.1225-f1,3,,1
wo<'constitution'>,f1.1226-f1,3,,1
wo<'contain'>,f1.1227-f1,3,,1
wo<'continent'>,f1.1228-f1,3,,1
wo<'continue'>,f1.1229-f1,3,,1
wo<'contribution'>,f1.1230-f1,3,,1
wo<'convalescence'>,f1.1231-f1,3,,1
wo<'convene'>,f1.1232-f1,3,,1
wo<'convenience'>,f1.1233-f1,3,,1
wo<'convenient'>,f1.1234-f1,3,,1
wo<'conventional'>,f1.1235-f1,3,,1
wo<'conversation'>,f1.1236-f1,3,,1
wo<'conversion'>,f1.1237-f1,3,,1
wo<'convert'>,f1.1238-f1,3,,1
wo<'convex'>,f1.1239-f1,3,,1
wo<'convey'>,f1.1240-f1,3,,1
wo<'convict'>,f1.1241-f1,3,,1
wo<'convince'>,f1.1242-f1,3,,1
wo<'convincing'>,f1.1243-f1,3,,1
wo<'cool'>,f1.1244-f1,3,,1
wo<'cope'>,f1.1245-f1,3,,1
wo<'copper'>,f1.1246-f1,3,,1
wo<'copy'>,f1.1247-f1,3,,1
wo<'copy-book'>,f1.1248-f1,3,,1
wo<'copyright'>,f1.1249-f1,3,,1
wo<'cord'>,f1.1250-f1,3,,1
wo<'cordial'>,f1.1251-f1,3,,1
wo<'core'>,f1.1252-f1,3,,1
wo<'cork'>,f1.1253-f1,3,,1
wo<'cork-screw'>,f1.1254-f1,3,,1
wo<'corn'>,f1.1255-f1,3,,1
wo<'corn flower'>,f1.1256-f1,3,,1
wo<'corner'>,f1.1257-f1,3,,1
wo<'corps'>,f1.1258-f1,3,,1
wo<'corpse'>,f1.1259-f1,3,,1
wo<'correct'>,f1.1260-f1,3,,1
wo<'correction'>,f1.1261-f1,3,,1
wo<'correctly'>,f1.1262-f1,3,,1
wo<'cosmodrome'>,f1.1263-f1,3,,1
wo<'cosmonaut'>,f1.1264-f1,3,,1
wo<'cosmonautics'>,f1.1265-f1,3,,1
wo<'cosmos'>,f1.1266-f1,3,,1
wo<'cossack'>,f1.1267-f1,3,,1
wo<'cost'>,f1.1268-f1,3,,1
wo<'cosy'>,f1.1269-f1,3,,1
wo<'cottage'>,f1.1270-f1,3,,1
wo<'cotton'>,f1.1271-f1,3,,1
wo<'cotton wool'>,f1.1272-f1,3,,1
wo<'counsel'>,f1.1273-f1,3,,1
wo<'count'>,f1.1274-f1,3,,1
wo<'counter'>,f1.1275-f1,3,,1
wo<'country'>,f1.1276-f1,3,,1
wo<'courage'>,f1.1277-f1,3,,1
wo<'courageous'>,f1.1278-f1,3,,1
wo<'course'>,f1.1279-f1,3,,1
wo<'cover'>,f1.1280-f1,3,,1
wo<'covet'>,f1.1281-f1,3,,1
wo<'covetous'>,f1.1282-f1,3,,1
wo<'cow'>,f1.1283-f1,3,,1
wo<'coward'>,f1.1284-f1,3,,1
wo<'crab'>,f1.1285-f1,3,,1
wo<'crack'>,f1.1286-f1,3,,1
wo<'cracker'>,f1.1287-f1,3,,1
wo<'crackle'>,f1.1288-f1,3,,1
wo<'cradle'>,f1.1289-f1,3,,1
wo<'craft'>,f1.1290-f1,3,,1
wo<'craftsman'>,f1.1291-f1,3,,1
wo<'cram'>,f1.1292-f1,3,,1
wo<'crane'>,f1.1293-f1,3,,1
wo<'crank'>,f1.1294-f1,3,,1
wo<'crash'>,f1.1295-f1,3,,1
wo<'crawl'>,f1.1296-f1,3,,1
wo<'craze'>,f1.1297-f1,3,,1
wo<'crazy'>,f1.1298-f1,3,,1
wo<'creak'>,f1.1299-f1,3,,1
wo<'cream'>,f1.1300-f1,3,,1
wo<'create'>,f1.1301-f1,3,,1
wo<'creation'>,f1.1302-f1,3,,1
wo<'creative'>,f1.1303-f1,3,,1
wo<'creator'>,f1.1304-f1,3,,1
wo<'creature'>,f1.1305-f1,3,,1
wo<'credit'>,f1.1306-f1,3,,1
wo<'crew'>,f1.1307-f1,3,,1
wo<'cricket'>,f1.1308-f1,3,,1
wo<'crop'>,f1.1309-f1,3,,1
wo<'cross'>,f1.1310-f1,3,,1
wo<'cross out'>,f1.1311-f1,3,,1
wo<'cross-road'>,f1.1312-f1,3,,1
wo<'crossing'>,f1.1313-f1,3,,1
wo<'cry'>,f1.1314-f1,3,,1
wo<'cub'>,f1.1315-f1,3,,1
wo<'cube'>,f1.1316-f1,3,,1
wo<'cubic'>,f1.1317-f1,3,,1
wo<'cuckoo'>,f1.1318-f1,3,,1
wo<'cultural'>,f1.1319-f1,3,,1
wo<'culture'>,f1.1320-f1,3,,1
wo<'cup'>,f1.1321-f1,3,,1
wo<'cupboard'>,f1.1322-f1,3,,1
wo<'curds'>,f1.1323-f1,3,,1
wo<'cure'>,f1.1324-f1,3,,1
wo<'curly'>,f1.1325-f1,3,,1
wo<'current'>,f1.1326-f1,3,,1
wo<'curse'>,f1.1327-f1,3,,1
wo<'cursory'>,f1.1328-f1,3,,1
wo<'cursory reading'>,f1.1329-f1,3,,1
wo<'curtain'>,f1.1330-f1,3,,1
wo<'custom'>,f1.1331-f1,3,,1
wo<'cut'>,f1.1332-f1,3,,1
wo<'cut up'>,f1.1333-f1,3,,1
wo<'cycle'>,f1.1334-f1,3,,1
wo<'cyclist'>,f1.1335-f1,3,,1
wo<'dad'>,f1.1336-f1,3,,1
wo<'daddy'>,f1.1337-f1,3,,1
wo<'dagger'>,f1.1338-f1,3,,1
wo<'daily'>,f1.1339-f1,3,,1
wo<'dainty'>,f1.1340-f1,3,,1
wo<'dairy'>,f1.1341-f1,3,,1
wo<'dairy-farm'>,f1.1342-f1,3,,1
wo<'daisy'>,f1.1343-f1,3,,1
wo<'dam'>,f1.1344-f1,3,,1
wo<'damage'>,f1.1345-f1,3,,1
wo<'damp'>,f1.1346-f1,3,,1
wo<'dance'>,f1.1347-f1,3,,1
wo<'dancer'>,f1.1348-f1,3,,1
wo<'dandelion'>,f1.1349-f1,3,,1
wo<'danger'>,f1.1350-f1,3,,1
wo<'dangle'>,f1.1351-f1,3,,1
wo<'dark'>,f1.1352-f1,3,,1
wo<'dark-eyed'>,f1.1353-f1,3,,1
wo<'dark-haired'>,f1.1354-f1,3,,1
wo<'darling'>,f1.1355-f1,3,,1
wo<'darn'>,f1.1356-f1,3,,1
wo<'dash'>,f1.1357-f1,3,,1
wo<'data'>,f1.1358-f1,3,,1
wo<'date'>,f1.1359-f1,3,,1
wo<'daughter'>,f1.1360-f1,3,,1
wo<'day'>,f1.1361-f1,3,,1
wo<'day after day'>,f1.1362-f1,3,,1
wo<'day off'>,f1.1363-f1,3,,1
wo<'day-book'>,f1.1364-f1,3,,1
wo<'dead'>,f1.1365-f1,3,,1
wo<'dean'>,f1.1366-f1,3,,1
wo<'dear'>,f1.1367-f1,3,,1
wo<'death'>,f1.1368-f1,3,,1
wo<'debt'>,f1.1369-f1,3,,1
wo<'debtor'>,f1.1370-f1,3,,1
wo<'decade'>,f1.1371-f1,3,,1
wo<'decay'>,f1.1372-f1,3,,1
wo<'decide'>,f1.1373-f1,3,,1
wo<'decimal'>,f1.1374-f1,3,,1
wo<'decipher'>,f1.1375-f1,3,,1
wo<'decision'>,f1.1376-f1,3,,1
wo<'decisive'>,f1.1377-f1,3,,1
wo<'deck'>,f1.1378-f1,3,,1
wo<'declare'>,f1.1379-f1,3,,1
wo<'decorate'>,f1.1380-f1,3,,1
wo<'decoration'>,f1.1381-f1,3,,1
wo<'deed'>,f1.1382-f1,3,,1
wo<'deer'>,f1.1383-f1,3,,1
wo<'defeat'>,f1.1384-f1,3,,1
wo<'defect'>,f1.1385-f1,3,,1
wo<'defective'>,f1.1386-f1,3,,1
wo<'defence'>,f1.1387-f1,3,,1
wo<'defenceless'>,f1.1388-f1,3,,1
wo<'defend'>,f1.1389-f1,3,,1
wo<'defensive'>,f1.1390-f1,3,,1
wo<'deference'>,f1.1391-f1,3,,1
wo<'defiant'>,f1.1392-f1,3,,1
wo<'deficiency'>,f1.1393-f1,3,,1
wo<'define'>,f1.1394-f1,3,,1
wo<'definite'>,f1.1395-f1,3,,1
wo<'definition'>,f1.1396-f1,3,,1
wo<'deformed'>,f1.1397-f1,3,,1
wo<'degree'>,f1.1398-f1,3,,1
wo<'deity'>,f1.1399-f1,3,,1
wo<'delegate'>,f1.1400-f1,3,,1
wo<'demand'>,f1.1401-f1,3,,1
wo<'demonstrate'>,f1.1402-f1,3,,1
wo<'demonstration'>,f1.1403-f1,3,,1
wo<'den'>,f1.1404-f1,3,,1
wo<'denial'>,f1.1405-f1,3,,1
wo<'deny'>,f1.1406-f1,3,,1
wo<'depart'>,f1.1407-f1,3,,1
wo<'department'>,f1.1408-f1,3,,1
wo<'department store'>,f1.1409-f1,3,,1
wo<'depict'>,f1.1410-f1,3,,1
wo<'deplorable'>,f1.1411-f1,3,,1
wo<'deposit'>,f1.1412-f1,3,,1
wo<'depot'>,f1.1413-f1,3,,1
wo<'deprive'>,f1.1414-f1,3,,1
wo<'depth'>,f1.1415-f1,3,,1
wo<'deputy'>,f1.1416-f1,3,,1
wo<'describe'>,f1.1417-f1,3,,1
wo<'design'>,f1.1418-f1,3,,1
wo<'designer'>,f1.1419-f1,3,,1
wo<'desire'>,f1.1420-f1,3,,1
wo<'desk'>,f1.1421-f1,3,,1
wo<'despotic'>,f1.1422-f1,3,,1
wo<'destination'>,f1.1423-f1,3,,1
wo<'destiny'>,f1.1424-f1,3,,1
wo<'destroy'>,f1.1425-f1,3,,1
wo<'detachment'>,f1.1426-f1,3,,1
wo<'develop'>,f1.1427-f1,3,,1
wo<'development'>,f1.1428-f1,3,,1
wo<'deviation'>,f1.1429-f1,3,,1
wo<'device'>,f1.1430-f1,3,,1
wo<'devil'>,f1.1431-f1,3,,1
wo<'devise'>,f1.1432-f1,3,,1
wo<'devote'>,f1.1433-f1,3,,1
wo<'devotion'>,f1.1434-f1,3,,1
wo<'devour'>,f1.1435-f1,3,,1
wo<'dew'>,f1.1436-f1,3,,1
wo<'dexterity'>,f1.1437-f1,3,,1
wo<'diagram'>,f1.1438-f1,3,,1
wo<'dial'>,f1.1439-f1,3,,1
wo<'dialectical'>,f1.1440-f1,3,,1
wo<'dialectical materialism'>,f1.1441-f1,3,,1
wo<'dialogue'>,f1.1442-f1,3,,1
wo<'diameter'>,f1.1443-f1,3,,1
wo<'diamond'>,f1.1444-f1,3,,1
wo<'dictation'>,f1.1445-f1,3,,1
wo<'did'>,f1.1446-f1,3,,3
wo<'die'>,f1.1447-f1,3,,1
wo<'differ'>,f1.1448-f1,3,,1
wo<'different'>,f1.1449-f1,3,,1
wo<'difficult'>,f1.1450-f1,3,,1
wo<'dig'>,f1.1451-f1,3,,1
wo<'digest'>,f1.1452-f1,3,,1
wo<'dignified'>,f1.1453-f1,3,,1
wo<'dignity'>,f1.1454-f1,3,,1
wo<'diligence'>,f1.1455-f1,3,,1
wo<'diligent'>,f1.1456-f1,3,,1
wo<'dill'>,f1.1457-f1,3,,1
wo<'dimple'>,f1.1458-f1,3,,1
wo<'dine'>,f1.1459-f1,3,,1
wo<'dining-car'>,f1.1460-f1,3,,1
wo<'dining-room'>,f1.1461-f1,3,,1
wo<'dinner'>,f1.1462-f1,3,,1
wo<'dip'>,f1.1463-f1,3,,1
wo<'diploma'>,f1.1464-f1,3,,1
wo<'direct'>,f1.1465-f1,3,,1
wo<'direction'>,f1.1466-f1,3,,1
wo<'directly'>,f1.1467-f1,3,,1
wo<'directory'>,f1.1468-f1,3,,1
wo<'dirt'>,f1.1469-f1,3,,1
wo<'dirty'>,f1.1470-f1,3,,1
wo<'disabled'>,f1.1471-f1,3,,1
wo<'discover'>,f1.1472-f1,3,,1
wo<'discuss'>,f1.1473-f1,3,,1
wo<'disk'>,f1.1474-f1,3,,1
wo<'display'>,f1.1475-f1,3,,1
wo<'distance'>,f1.1476-f1,3,,1
wo<'divide'>,f1.1477-f1,3,,1
wo<'do'>,f1.1478-f1,3,,1
wo<'do away'>,f1.1479-f1,3,,1
wo<'do one',39,'s shopping'>,f1.1480-f1,3,,1
wo<'do sums'>,f1.1481-f1,3,,1
wo<'doctor'>,f1.1482-f1,3,,1
wo<'document'>,f1.1483-f1,3,,1
wo<'dog'>,f1.1484-f1,3,,1
wo<'dogged'>,f1.1485-f1,3,,1
wo<'doll'>,f1.1486-f1,3,,1
wo<'dollar'>,f1.1487-f1,3,,1
wo<'dome'>,f1.1488-f1,3,,1
wo<'door'>,f1.1489-f1,3,,1
wo<'doubly'>,f1.1490-f1,3,,1
wo<'dough'>,f1.1491-f1,3,,1
wo<'dove'>,f1.1492-f1,3,,1
wo<'down'>,f1.1493-f1,3,,1
wo<'doze'>,f1.1494-f1,3,,1
wo<'dozen'>,f1.1495-f1,3,,1
wo<'drag'>,f1.1496-f1,3,,1
wo<'dramatic'>,f1.1497-f1,3,,1
wo<'drank'>,f1.1498-f1,3,,3
wo<'drapery'>,f1.1499-f1,3,,1
wo<'drastic measures'>,f1.1500-f1,3,,1
wo<'drastic remedy'>,f1.1501-f1,3,,1
wo<'draught'>,f1.1502-f1,3,,1
wo<'draw'>,f1.1503-f1,3,,1
wo<'draw near'>,f1.1504-f1,3,,1
wo<'drawer'>,f1.1505-f1,3,,1
wo<'drawers'>,f1.1506-f1,3,,1
wo<'drawing'>,f1.1507-f1,3,,1
wo<'dreamed'>,f1.1508-f1,3,,3
wo<'dreamt'>,f1.1509-f1,3,,3
wo<'dreary'>,f1.1510-f1,3,,1
wo<'dress'>,f1.1511-f1,3,,1
wo<'dress-maker'>,f1.1512-f1,3,,1
wo<'drew'>,f1.1513-f1,3,,3
wo<'dried'>,f1.1514-f1,3,,1
wo<'drink'>,f1.1515-f1,3,,1
wo<'drive'>,f1.1516-f1,3,,1
wo<'driver'>,f1.1517-f1,3,,1
wo<'drove'>,f1.1518-f1,3,,3
wo<'dry'>,f1.1519-f1,3,,1
wo<'due'>,f1.1520-f1,3,,1
wo<'due to'>,f1.1521-f1,3,,1
wo<'dug'>,f1.1522-f1,3,,1
wo<'duke'>,f1.1523-f1,3,,1
wo<'dull'>,f1.1524-f1,3,,1
wo<'dull edge'>,f1.1525-f1,3,,1
wo<'dumb'>,f1.1526-f1,3,,1
wo<'dummy'>,f1.1527-f1,3,,1
wo<'dupe'>,f1.1528-f1,3,,1
wo<'duplicate'>,f1.1529-f1,3,,1
wo<'durable'>,f1.1530-f1,3,,1
wo<'duration'>,f1.1531-f1,3,,1
wo<'during'>,f1.1532-f1,3,,1
wo<'dusk'>,f1.1533-f1,3,,1
wo<'dust'>,f1.1534-f1,3,,1
wo<'duster'>,f1.1535-f1,3,,1
wo<'dusty'>,f1.1536-f1,3,,1
wo<'duty'>,f1.1537-f1,3,,1
wo<'dwarf'>,f1.1538-f1,3,,1
wo<'dwell'>,f1.1539-f1,3,,1
wo<'dwell on'>,f1.1540-f1,3,,1
wo<'dwelling'>,f1.1541-f1,3,,1
wo<'dye'>,f1.1542-f1,3,,1
wo<'dyed'>,f1.1543-f1,3,,1
wo<'dying'>,f1.1544-f1,3,,1
wo<'dysentery'>,f1.1545-f1,3,,1
wo<'each'>,f1.1546-f1,3,,1
wo<'each other'>,f1.1547-f1,3,,1
wo<'eager'>,f1.1548-f1,3,,1
wo<'eagle'>,f1.1549-f1,3,,1
wo<'ear'>,f1.1550-f1,3,,1
wo<'early'>,f1.1551-f1,3,,1
wo<'earn'>,f1.1552-f1,3,,1
wo<'earnest'>,f1.1553-f1,3,,1
wo<'earnings'>,f1.1554-f1,3,,1
wo<'earth'>,f1.1555-f1,3,,1
wo<'earthquake'>,f1.1556-f1,3,,1
wo<'ease'>,f1.1557-f1,3,,1
wo<'easel'>,f1.1558-f1,3,,1
wo<'easily'>,f1.1559-f1,3,,1
wo<'east'>,f1.1560-f1,3,,1
wo<'eastern'>,f1.1561-f1,3,,1
wo<'easy'>,f1.1562-f1,3,,1
wo<'eat'>,f1.1563-f1,3,,1
wo<'economy'>,f1.1564-f1,3,,1
wo<'education'>,f1.1565-f1,3,,1
wo<'effort'>,f1.1566-f1,3,,1
wo<'eight'>,f1.1567-f1,3,,1
wo<'eighth'>,f1.1568-f1,3,,1
wo<'elder'>,f1.1569-f1,3,,1
wo<'elect'>,f1.1570-f1,3,,1
wo<'electric'>,f1.1571-f1,3,,1
wo<'electric power station'>,f1.1572-f1,3,,1
wo<'elegant'>,f1.1573-f1,3,,1
wo<'element'>,f1.1574-f1,3,,1
wo<'elementary'>,f1.1575-f1,3,,1
wo<'elephant'>,f1.1576-f1,3,,1
wo<'elevate'>,f1.1577-f1,3,,1
wo<'eleven'>,f1.1578-f1,3,,1
wo<'eleventh'>,f1.1579-f1,3,,1
wo<'else'>,f1.1580-f1,3,,1
wo<'embassy'>,f1.1581-f1,3,,1
wo<'embryo'>,f1.1582-f1,3,,1
wo<'emerald'>,f1.1583-f1,3,,1
wo<'emerge'>,f1.1584-f1,3,,1
wo<'end'>,f1.1585-f1,3,,1
wo<'enemy'>,f1.1586-f1,3,,1
wo<'engine'>,f1.1587-f1,3,,1
wo<'engineer'>,f1.1588-f1,3,,1
wo<'enjoy'>,f1.1589-f1,3,,1
wo<'enlarge'>,f1.1590-f1,3,,1
wo<'enlighten'>,f1.1591-f1,3,,1
wo<'enlist'>,f1.1592-f1,3,,1
wo<'enmity'>,f1.1593-f1,3,,1
wo<'enormous'>,f1.1594-f1,3,,1
wo<'enough'>,f1.1595-f1,3,,1
wo<'enrich'>,f1.1596-f1,3,,1
wo<'enslave'>,f1.1597-f1,3,,1
wo<'entangled'>,f1.1598-f1,3,,1
wo<'enter'>,f1.1599-f1,3,,1
wo<'entice'>,f1.1600-f1,3,,1
wo<'entry'>,f1.1601-f1,3,,1
wo<'enumerate'>,f1.1602-f1,3,,1
wo<'envelop'>,f1.1603-f1,3,,1
wo<'envelope'>,f1.1604-f1,3,,1
wo<'envious'>,f1.1605-f1,3,,1
wo<'environment'>,f1.1606-f1,3,,1
wo<'epoch'>,f1.1607-f1,3,,1
wo<'erase'>,f1.1608-f1,3,,1
wo<'erect'>,f1.1609-f1,3,,1
wo<'especially'>,f1.1610-f1,3,,1
wo<'essay'>,f1.1611-f1,3,,1
wo<'establish'>,f1.1612-f1,3,,1
wo<'eternity'>,f1.1613-f1,3,,1
wo<'ether'>,f1.1614-f1,3,,1
wo<'eve'>,f1.1615-f1,3,,1
wo<'even'>,f1.1616-f1,3,,1
wo<'event'>,f1.1617-f1,3,,1
wo<'ever'>,f1.1618-f1,3,,1
wo<'everly other day'>,f1.1619-f1,3,,1
wo<'every'>,f1.1620-f1,3,,1
wo<'everybody'>,f1.1621-f1,3,,1
wo<'everyone'>,f1.1622-f1,3,,1
wo<'everything'>,f1.1623-f1,3,,1
wo<'evil'>,f1.1624-f1,3,,1
wo<'evince'>,f1.1625-f1,3,,1
wo<'evoke'>,f1.1626-f1,3,,1
wo<'exact'>,f1.1627-f1,3,,1
wo<'exactly'>,f1.1628-f1,3,,1
wo<'exaggerate'>,f1.1629-f1,3,,1
wo<'exalt'>,f1.1630-f1,3,,1
wo<'examination'>,f1.1631-f1,3,,1
wo<'examine'>,f1.1632-f1,3,,1
wo<'example'>,f1.1633-f1,3,,1
wo<'excel'>,f1.1634-f1,3,,1
wo<'excellent'>,f1.1635-f1,3,,1
wo<'excite'>,f1.1636-f1,3,,1
wo<'excitement'>,f1.1637-f1,3,,1
wo<'exclaim'>,f1.1638-f1,3,,1
wo<'exclamation'>,f1.1639-f1,3,,1
wo<'exclude'>,f1.1640-f1,3,,1
wo<'excursion'>,f1.1641-f1,3,,1
wo<'exercise'>,f1.1642-f1,3,,1
wo<'exercise-book'>,f1.1643-f1,3,,1
wo<'exhibition'>,f1.1644-f1,3,,1
wo<'exit'>,f1.1645-f1,3,,1
wo<'expand'>,f1.1646-f1,3,,1
wo<'expansion'>,f1.1647-f1,3,,1
wo<'expect'>,f1.1648-f1,3,,1
wo<'expectation'>,f1.1649-f1,3,,1
wo<'expedition'>,f1.1650-f1,3,,1
wo<'expel'>,f1.1651-f1,3,,1
wo<'expenditure'>,f1.1652-f1,3,,1
wo<'experiment'>,f1.1653-f1,3,,1
wo<'explain'>,f1.1654-f1,3,,1
wo<'exploration'>,f1.1655-f1,3,,1
wo<'explore'>,f1.1656-f1,3,,1
wo<'expression'>,f1.1657-f1,3,,1
wo<'exratriate'>,f1.1658-f1,3,,1
wo<'extra'>,f1.1659-f1,3,,1
wo<'eye'>,f1.1660-f1,3,,1
wo<'fable'>,f1.1661-f1,3,,1
wo<'fabric'>,f1.1662-f1,3,,1
wo<'face'>,f1.1663-f1,3,,1
wo<'face of the clock'>,f1.1664-f1,3,,1
wo<'facetious'>,f1.1665-f1,3,,1
wo<'facility'>,f1.1666-f1,3,,1
wo<'fact'>,f1.1667-f1,3,,1
wo<'factory'>,f1.1668-f1,3,,1
wo<'fade'>,f1.1669-f1,3,,1
wo<'fail'>,f1.1670-f1,3,,1
wo<'failure'>,f1.1671-f1,3,,1
wo<'fair'>,f1.1672-f1,3,,1
wo<'fair-haired'>,f1.1673-f1,3,,1
wo<'fairy-tale'>,f1.1674-f1,3,,1
wo<'fall'>,f1.1675-f1,3,,1
wo<'fall asleep'>,f1.1676-f1,3,,1
wo<'fall ill'>,f1.1677-f1,3,,1
wo<'family'>,f1.1678-f1,3,,1
wo<'famous'>,f1.1679-f1,3,,1
wo<'far'>,f1.1680-f1,3,,1
wo<'farm'>,f1.1681-f1,3,,1
wo<'farmer'>,f1.1682-f1,3,,1
wo<'fate'>,f1.1683-f1,3,,1
wo<'father'>,f1.1684-f1,3,,1
wo<'fault'>,f1.1685-f1,3,,1
wo<'favour'>,f1.1686-f1,3,,1
wo<'favourite'>,f1.1687-f1,3,,1
wo<'feature'>,f1.1688-f1,3,,1
wo<'fed'>,f1.1689-f1,3,,1
wo<'federal'>,f1.1690-f1,3,,1
wo<'federative'>,f1.1691-f1,3,,1
wo<'fee'>,f1.1692-f1,3,,1
wo<'feeble'>,f1.1693-f1,3,,1
wo<'feed'>,f1.1694-f1,3,,1
wo<'feel'>,f1.1695-f1,3,,1
wo<'feel queer'>,f1.1696-f1,3,,1
wo<'feeling'>,f1.1697-f1,3,,1
wo<'feet'>,f1.1698-f1,3,,1
wo<'fell'>,f1.1699-f1,3,,1
wo<'felt'>,f1.1700-f1,3,,1
wo<'fence'>,f1.1701-f1,3,,1
wo<'few'>,f1.1702-f1,3,,1
wo<'field'>,f1.1703-f1,3,,1
wo<'fierce'>,f1.1704-f1,3,,1
wo<'fifteen'>,f1.1705-f1,3,,1
wo<'fifth'>,f1.1706-f1,3,,1
wo<'fiftieth'>,f1.1707-f1,3,,1
wo<'fifty'>,f1.1708-f1,3,,1
wo<'fight'>,f1.1709-f1,3,,1
wo<'fighter'>,f1.1710-f1,3,,1
wo<'figure'>,f1.1711-f1,3,,1
wo<'figure skating'>,f1.1712-f1,3,,1
wo<'file'>,f1.1713-f1,3,,1
wo<'fill'>,f1.1714-f1,3,,1
wo<'film'>,f1.1715-f1,3,,1
wo<'find'>,f1.1716-f1,3,,1
wo<'fine'>,f1.1717-f1,3,,1
wo<'finish'>,f1.1718-f1,3,,1
wo<'fir'>,f1.1719-f1,3,,1
wo<'fire'>,f1.1720-f1,3,,1
wo<'fire-place'>,f1.1721-f1,3,,1
wo<'first'>,f1.1722-f1,3,,1
wo<'fish'>,f1.1723-f1,3,,1
wo<'fit'>,f1.1724-f1,3,,1
wo<'five'>,f1.1725-f1,3,,1
wo<'fix'>,f1.1726-f1,3,,1
wo<'fixed'>,f1.1727-f1,3,,1
wo<'flag'>,f1.1728-f1,3,,1
wo<'flask'>,f1.1729-f1,3,,1
wo<'flax'>,f1.1730-f1,3,,1
wo<'flea'>,f1.1731-f1,3,,1
wo<'flew'>,f1.1732-f1,3,,3
wo<'flight'>,f1.1733-f1,3,,1
wo<'floor'>,f1.1734-f1,3,,1
wo<'flower'>,f1.1735-f1,3,,1
wo<'fly'>,f1.1736-f1,3,,1
wo<'foal'>,f1.1737-f1,3,,1
wo<'foam'>,f1.1738-f1,3,,1
wo<'folk'>,f1.1739-f1,3,,1
wo<'follow'>,f1.1740-f1,3,,1
wo<'fond'>,f1.1741-f1,3,,1
wo<'food'>,f1.1742-f1,3,,1
wo<'fool'>,f1.1743-f1,3,,1
wo<'foot'>,f1.1744-f1,3,,1
wo<'foot-note'>,f1.1745-f1,3,,1
wo<'football'>,f1.1746-f1,3,,1
wo<'football player'>,f1.1747-f1,3,,1
wo<'footstep'>,f1.1748-f1,3,,1
wo<'for'>,f1.1749-f1,3,,1
wo<'for ages'>,f1.1750-f1,3,,1
wo<'for all'>,f1.1751-f1,3,,1
wo<'for breakfast'>,f1.1752-f1,3,,1
wo<'for certain'>,f1.1753-f1,3,,1
wo<'for example'>,f1.1754-f1,3,,1
wo<'for hire'>,f1.1755-f1,3,,1
wo<'force'>,f1.1756-f1,3,,1
wo<'forefather'>,f1.1757-f1,3,,1
wo<'forest'>,f1.1758-f1,3,,1
wo<'forgave'>,f1.1759-f1,3,,3
wo<'forget'>,f1.1760-f1,3,,1
wo<'forgot'>,f1.1761-f1,3,,3
wo<'fork'>,f1.1762-f1,3,,1
wo<'form'>,f1.1763-f1,3,,1
wo<'formula'>,f1.1764-f1,3,,1
wo<'forth'>,f1.1765-f1,3,,1
wo<'forth-coming'>,f1.1766-f1,3,,1
wo<'fortieth'>,f1.1767-f1,3,,1
wo<'forty'>,f1.1768-f1,3,,1
wo<'fought'>,f1.1769-f1,3,,1
wo<'foul'>,f1.1770-f1,3,,1
wo<'foul play'>,f1.1771-f1,3,,1
wo<'found'>,f1.1772-f1,3,,1
wo<'founder'>,f1.1773-f1,3,,1
wo<'four'>,f1.1774-f1,3,,1
wo<'fourth'>,f1.1775-f1,3,,1
wo<'fowl'>,f1.1776-f1,3,,1
wo<'fox'>,f1.1777-f1,3,,1
wo<'frame'>,f1.1778-f1,3,,1
wo<'frame-up'>,f1.1779-f1,3,,1
wo<'framework'>,f1.1780-f1,3,,1
wo<'framework of society'>,f1.1781-f1,3,,1
wo<'frank'>,f1.1782-f1,3,,1
wo<'frankly'>,f1.1783-f1,3,,1
wo<'frankly speaking'>,f1.1784-f1,3,,1
wo<'fraternal'>,f1.1785-f1,3,,1
wo<'free'>,f1.1786-f1,3,,1
wo<'free of charge'>,f1.1787-f1,3,,1
wo<'freedom'>,f1.1788-f1,3,,1
wo<'fresh'>,f1.1789-f1,3,,1
wo<'friend'>,f1.1790-f1,3,,1
wo<'friendly'>,f1.1791-f1,3,,1
wo<'friendship'>,f1.1792-f1,3,,1
wo<'from'>,f1.1793-f1,3,,1
wo<'front'>,f1.1794-f1,3,,1
wo<'frost'>,f1.1795-f1,3,,1
wo<'frosty'>,f1.1796-f1,3,,1
wo<'froze'>,f1.1797-f1,3,,3
wo<'fruit'>,f1.1798-f1,3,,1
wo<'full stop'>,f1.1799-f1,3,,1
wo<'funny'>,f1.1800-f1,3,,1
wo<'gad-fly'>,f1.1801-f1,3,,1
wo<'gaily'>,f1.1802-f1,3,,1
wo<'gain'>,f1.1803-f1,3,,1
wo<'gait'>,f1.1804-f1,3,,1
wo<'gallant'>,f1.1805-f1,3,,1
wo<'gallery'>,f1.1806-f1,3,,1
wo<'gallows'>,f1.1807-f1,3,,1
wo<'galoshes'>,f1.1808-f1,3,,1
wo<'gamble'>,f1.1809-f1,3,,1
wo<'gambler'>,f1.1810-f1,3,,1
wo<'game'>,f1.1811-f1,3,,1
wo<'gang'>,f1.1812-f1,3,,1
wo<'gangster'>,f1.1813-f1,3,,1
wo<'gap'>,f1.1814-f1,3,,1
wo<'gape'>,f1.1815-f1,3,,1
wo<'garbage'>,f1.1816-f1,3,,1
wo<'garden'>,f1.1817-f1,3,,1
wo<'gardener'>,f1.1818-f1,3,,1
wo<'gas'>,f1.1819-f1,3,,1
wo<'gather'>,f1.1820-f1,3,,1
wo<'gave'>,f1.1821-f1,3,,3
wo<'gaze'>,f1.1822-f1,3,,1
wo<'gender'>,f1.1823-f1,3,,1
wo<'general'>,f1.1824-f1,3,,1
wo<'generally'>,f1.1825-f1,3,,1
wo<'generation'>,f1.1826-f1,3,,1
wo<'genius'>,f1.1827-f1,3,,1
wo<'gentle'>,f1.1828-f1,3,,1
wo<'geography'>,f1.1829-f1,3,,1
wo<'get'>,f1.1830-f1,3,,1
wo<'get better'>,f1.1831-f1,3,,1
wo<'get in'>,f1.1832-f1,3,,1
wo<'get off'>,f1.1833-f1,3,,1
wo<'get on'>,f1.1834-f1,3,,1
wo<'get up'>,f1.1835-f1,3,,1
wo<'get warm'>,f1.1836-f1,3,,1
wo<'get wet'>,f1.1837-f1,3,,1
wo<'ghost'>,f1.1838-f1,3,,1
wo<'giant'>,f1.1839-f1,3,,1
wo<'gigantic'>,f1.1840-f1,3,,1
wo<'gild'>,f1.1841-f1,3,,1
wo<'gilt'>,f1.1842-f1,3,,1
wo<'girl'>,f1.1843-f1,3,,1
wo<'give'>,f1.1844-f1,3,,1
wo<'glad'>,f1.1845-f1,3,,1
wo<'glider'>,f1.1846-f1,3,,1
wo<'go'>,f1.1847-f1,3,,1
wo<'go by bus'>,f1.1848-f1,3,,1
wo<'go by rail'>,f1.1849-f1,3,,1
wo<'go for a walk'>,f1.1850-f1,3,,1
wo<'go home'>,f1.1851-f1,3,,1
wo<'go in for sports'>,f1.1852-f1,3,,1
wo<'go on foot'>,f1.1853-f1,3,,1
wo<'go out'>,f1.1854-f1,3,,1
wo<'go shopping'>,f1.1855-f1,3,,1
wo<'go to bed'>,f1.1856-f1,3,,1
wo<'goal'>,f1.1857-f1,3,,1
wo<'goat'>,f1.1858-f1,3,,1
wo<'god'>,f1.1859-f1,3,,1
wo<'goggles'>,f1.1860-f1,3,,1
wo<'gold'>,f1.1861-f1,3,,1
wo<'golden'>,f1.1862-f1,3,,1
wo<'good'>,f1.1863-f1,3,,1
wo<'good afternoon'>,f1.1864-f1,3,,1
wo<'good-by'>,f1.1865-f1,3,,1
wo<'goods'>,f1.1866-f1,3,,1
wo<'goose'>,f1.1867-f1,3,,1
wo<'gooseberry'>,f1.1868-f1,3,,1
wo<'got'>,f1.1869-f1,3,,1
wo<'govern'>,f1.1870-f1,3,,1
wo<'government'>,f1.1871-f1,3,,1
wo<'governor'>,f1.1872-f1,3,,1
wo<'gown'>,f1.1873-f1,3,,1
wo<'grain'>,f1.1874-f1,3,,1
wo<'grand'>,f1.1875-f1,3,,1
wo<'grand meeting'>,f1.1876-f1,3,,1
wo<'grandfather'>,f1.1877-f1,3,,1
wo<'grandmother'>,f1.1878-f1,3,,1
wo<'grass'>,f1.1879-f1,3,,1
wo<'grating'>,f1.1880-f1,3,,1
wo<'grave'>,f1.1881-f1,3,,1
wo<'great'>,f1.1882-f1,3,,1
wo<'green'>,f1.1883-f1,3,,1
wo<'greet'>,f1.1884-f1,3,,1
wo<'greeting'>,f1.1885-f1,3,,1
wo<'grew'>,f1.1886-f1,3,,3
wo<'grey'>,f1.1887-f1,3,,1
wo<'grey-eyed'>,f1.1888-f1,3,,1
wo<'grey-haired'>,f1.1889-f1,3,,1
wo<'grip'>,f1.1890-f1,3,,1
wo<'groove'>,f1.1891-f1,3,,1
wo<'gross'>,f1.1892-f1,3,,1
wo<'ground'>,f1.1893-f1,3,,1
wo<'group'>,f1.1894-f1,3,,1
wo<'grow'>,f1.1895-f1,3,,1
wo<'growl'>,f1.1896-f1,3,,1
wo<'grown-up'>,f1.1897-f1,3,,1
wo<'grownup'>,f1.1898-f1,3,,1
wo<'growth'>,f1.1899-f1,3,,1
wo<'guaranteed'>,f1.1900-f1,3,,1
wo<'guest'>,f1.1901-f1,3,,1
wo<'guide'>,f1.1902-f1,3,,1
wo<'gull'>,f1.1903-f1,3,,1
wo<'gum'>,f1.1904-f1,3,,1
wo<'gun'>,f1.1905-f1,3,,1
wo<'gust'>,f1.1906-f1,3,,1
wo<'guts'>,f1.1907-f1,3,,1
wo<'gutter'>,f1.1908-f1,3,,1
wo<'gymnasium'>,f1.1909-f1,3,,1
wo<'gymnastics'>,f1.1910-f1,3,,1
wo<'haberdashery'>,f1.1911-f1,3,,1
wo<'habit'>,f1.1912-f1,3,,1
wo<'habitual'>,f1.1913-f1,3,,1
wo<'had'>,f1.1914-f1,3,,1
wo<'hair'>,f1.1915-f1,3,,1
wo<'hairy'>,f1.1916-f1,3,,1
wo<'half'>,f1.1917-f1,3,,1
wo<'hand'>,f1.1918-f1,3,,1
wo<'handsome'>,f1.1919-f1,3,,1
wo<'hang'>,f1.1920-f1,3,,1
wo<'hanged'>,f1.1921-f1,3,,1
wo<'hangry'>,f1.1922-f1,3,,1
wo<'happen'>,f1.1923-f1,3,,1
wo<'happily'>,f1.1924-f1,3,,1
wo<'happy'>,f1.1925-f1,3,,1
wo<'hard'>,f1.1926-f1,3,,1
wo<'hare'>,f1.1927-f1,3,,1
wo<'harm'>,f1.1928-f1,3,,1
wo<'harmless'>,f1.1929-f1,3,,1
wo<'harmonious'>,f1.1930-f1,3,,1
wo<'harmony'>,f1.1931-f1,3,,1
wo<'harness'>,f1.1932-f1,3,,1
wo<'harrow'>,f1.1933-f1,3,,1
wo<'harvest'>,f1.1934-f1,3,,1
wo<'hat'>,f1.1935-f1,3,,1
wo<'hate'>,f1.1936-f1,3,,1
wo<'have'>,f1.1937-f1,3,,1
wo<'have a good time'>,f1.1938-f1,3,,1
wo<'have a rest'>,f1.1939-f1,3,,1
wo<'have breakfast'>,f1.1940-f1,3,,1
wo<'have dinner'>,f1.1941-f1,3,,1
wo<'have supper'>,f1.1942-f1,3,,1
wo<'have to do something'>,f1.1943-f1,3,,1
wo<'he'>,f1.1944-f1,3,,1
wo<'he is anxious to see you'>,f1.1945-f1,3,,1
wo<'he is away'>,f1.1946-f1,3,,1
wo<'he is out'>,f1.1947-f1,3,,1
wo<'head'>,f1.1948-f1,3,,1
wo<'headmaster'>,f1.1949-f1,3,,1
wo<'headquarters'>,f1.1950-f1,3,,1
wo<'health'>,f1.1951-f1,3,,1
wo<'hear'>,f1.1952-f1,3,,1
wo<'heard'>,f1.1953-f1,3,,1
wo<'heart'>,f1.1954-f1,3,,1
wo<'heavy'>,f1.1955-f1,3,,1
wo<'height'>,f1.1956-f1,3,,1
wo<'heir'>,f1.1957-f1,3,,1
wo<'held'>,f1.1958-f1,3,,3
wo<'helicopter'>,f1.1959-f1,3,,1
wo<'hell'>,f1.1960-f1,3,,1
wo<'helm'>,f1.1961-f1,3,,1
wo<'helmet'>,f1.1962-f1,3,,1
wo<'help'>,f1.1963-f1,3,,1
wo<'hemisphere'>,f1.1964-f1,3,,1
wo<'her'>,f1.1965-f1,3,,1
wo<'here'>,f1.1966-f1,3,,1
wo<'here are you'>,f1.1967-f1,3,,1
wo<'hero'>,f1.1968-f1,3,,1
wo<'heroic'>,f1.1969-f1,3,,1
wo<'heroism'>,f1.1970-f1,3,,1
wo<'herself'>,f1.1971-f1,3,,1
wo<'hid'>,f1.1972-f1,3,,3
wo<'hide'>,f1.1973-f1,3,,1
wo<'hideous'>,f1.1974-f1,3,,1
wo<'high'>,f1.1975-f1,3,,1
wo<'high school'>,f1.1976-f1,3,,1
wo<'higher education'>,f1.1977-f1,3,,1
wo<'highly'>,f1.1978-f1,3,,1
wo<'highway'>,f1.1979-f1,3,,1
wo<'hill'>,f1.1980-f1,3,,1
wo<'him'>,f1.1981-f1,3,,1
wo<'himself'>,f1.1982-f1,3,,1
wo<'hinder'>,f1.1983-f1,3,,1
wo<'hindrance'>,f1.1984-f1,3,,1
wo<'hinge'>,f1.1985-f1,3,,1
wo<'hinge on'>,f1.1986-f1,3,,1
wo<'hint'>,f1.1987-f1,3,,1
wo<'hip'>,f1.1988-f1,3,,1
wo<'hire'>,f1.1989-f1,3,,1
wo<'his'>,f1.1990-f1,3,,1
wo<'history'>,f1.1991-f1,3,,1
wo<'hit'>,f1.1992-f1,3,,1
wo<'hobby'>,f1.1993-f1,3,,1
wo<'hockey'>,f1.1994-f1,3,,1
wo<'hockey player'>,f1.1995-f1,3,,1
wo<'hold'>,f1.1996-f1,3,,1
wo<'hold aloof from'>,f1.1997-f1,3,,1
wo<'hold keep aloof from'>,f1.1998-f1,3,,1
wo<'holiday'>,f1.1999-f1,3,,1
wo<'holidays'>,f1.2000-f1,3,,1
wo<'home'>,f1.2001-f1,3,,1
wo<'hometask'>,f1.2002-f1,3,,1
wo<'homework'>,f1.2003-f1,3,,1
wo<'honest'>,f1.2004-f1,3,,1
wo<'honour'>,f1.2005-f1,3,,1
wo<'honour the memory'>,f1.2006-f1,3,,1
wo<'hope'>,f1.2007-f1,3,,1
wo<'horse'>,f1.2008-f1,3,,1
wo<'hospital'>,f1.2009-f1,3,,1
wo<'hot'>,f1.2010-f1,3,,1
wo<'hotel'>,f1.2011-f1,3,,1
wo<'hour'>,f1.2012-f1,3,,1
wo<'house'>,f1.2013-f1,3,,1
wo<'housewife'>,f1.2014-f1,3,,1
wo<'how'>,f1.2015-f1,3,,1
wo<'how do you do'>,f1.2016-f1,3,,1
wo<'how many'>,f1.2017-f1,3,,1
wo<'how much'>,f1.2018-f1,3,,1
wo<'how old are you'>,f1.2019-f1,3,,1
wo<'huge'>,f1.2020-f1,3,,1
wo<'hundred'>,f1.2021-f1,3,,1
wo<'hundred and first'>,f1.2022-f1,3,,1
wo<'hundredth'>,f1.2023-f1,3,,1
wo<'hung'>,f1.2024-f1,3,,1
wo<'hurt'>,f1.2025-f1,3,,1
wo<'husband'>,f1.2026-f1,3,,1
wo<'ice'>,f1.2027-f1,3,,1
wo<'ice-cream'>,f1.2028-f1,3,,1
wo<'idea'>,f1.2029-f1,3,,1
wo<'ideal'>,f1.2030-f1,3,,1
wo<'identical'>,f1.2031-f1,3,,1
wo<'identify'>,f1.2032-f1,3,,1
wo<'ideology'>,f1.2033-f1,3,,1
wo<'idiot'>,f1.2034-f1,3,,1
wo<'idle'>,f1.2035-f1,3,,1
wo<'idol'>,f1.2036-f1,3,,1
wo<'if'>,f1.2037-f1,3,,1
wo<'ill'>,f1.2038-f1,3,,1
wo<'impel'>,f1.2039-f1,3,,1
wo<'impenetrable'>,f1.2040-f1,3,,1
wo<'implement'>,f1.2041-f1,3,,1
wo<'implore'>,f1.2042-f1,3,,1
wo<'imply'>,f1.2043-f1,3,,1
wo<'import'>,f1.2044-f1,3,,1
wo<'importance'>,f1.2045-f1,3,,1
wo<'important'>,f1.2046-f1,3,,1
wo<'impression'>,f1.2047-f1,3,,1
wo<'in'>,f1.2048-f1,3,,1
wo<'in a body'>,f1.2049-f1,3,,1
wo<'in accordance with'>,f1.2050-f1,3,,1
wo<'in comparison with'>,f1.2051-f1,3,,1
wo<'in compliance with'>,f1.2052-f1,3,,1
wo<'in conformity with'>,f1.2053-f1,3,,1
wo<'in fact'>,f1.2054-f1,3,,1
wo<'in favour of'>,f1.2055-f1,3,,1
wo<'in front of'>,f1.2056-f1,3,,1
wo<'in general'>,f1.2057-f1,3,,1
wo<'in my capacity as a doctor'>,f1.2058-f1,3,,1
wo<'in respect to'>,f1.2059-f1,3,,1
wo<'in spite of'>,f1.2060-f1,3,,1
wo<'in the country'>,f1.2061-f1,3,,1
wo<'in the main'>,f1.2062-f1,3,,1
wo<'in the meadow'>,f1.2063-f1,3,,1
wo<'in the morning'>,f1.2064-f1,3,,1
wo<'in the picture'>,f1.2065-f1,3,,1
wo<'in vain'>,f1.2066-f1,3,,1
wo<'inability'>,f1.2067-f1,3,,1
wo<'inaccessible'>,f1.2068-f1,3,,1
wo<'inadequate'>,f1.2069-f1,3,,1
wo<'inch'>,f1.2070-f1,3,,1
wo<'include'>,f1.2071-f1,3,,1
wo<'including'>,f1.2072-f1,3,,1
wo<'increase'>,f1.2073-f1,3,,1
wo<'indecent'>,f1.2074-f1,3,,1
wo<'indeed'>,f1.2075-f1,3,,1
wo<'indefinite'>,f1.2076-f1,3,,1
wo<'independence'>,f1.2077-f1,3,,1
wo<'independent'>,f1.2078-f1,3,,1
wo<'index'>,f1.2079-f1,3,,1
wo<'industrial'>,f1.2080-f1,3,,1
wo<'industry'>,f1.2081-f1,3,,1
wo<'inflation'>,f1.2082-f1,3,,1
wo<'influence'>,f1.2083-f1,3,,1
wo<'inform'>,f1.2084-f1,3,,1
wo<'informal'>,f1.2085-f1,3,,1
wo<'information'>,f1.2086-f1,3,,1
wo<'inhale'>,f1.2087-f1,3,,1
wo<'inherit'>,f1.2088-f1,3,,1
wo<'inheritance'>,f1.2089-f1,3,,1
wo<'inhuman'>,f1.2090-f1,3,,1
wo<'initiative'>,f1.2091-f1,3,,1
wo<'injure'>,f1.2092-f1,3,,1
wo<'institute'>,f1.2093-f1,3,,1
wo<'integral'>,f1.2094-f1,3,,1
wo<'interest'>,f1.2095-f1,3,,1
wo<'interesting'>,f1.2096-f1,3,,1
wo<'international'>,f1.2097-f1,3,,1
wo<'interval'>,f1.2098-f1,3,,1
wo<'intervention'>,f1.2099-f1,3,,1
wo<'into'>,f1.2100-f1,3,,1
wo<'intolerant'>,f1.2101-f1,3,,1
wo<'intoxicate'>,f1.2102-f1,3,,1
wo<'introduce'>,f1.2103-f1,3,,1
wo<'inventor'>,f1.2104-f1,3,,1
wo<'invite'>,f1.2105-f1,3,,1
wo<'iron'>,f1.2106-f1,3,,1
wo<'ironclad'>,f1.2107-f1,3,,1
wo<'island'>,f1.2108-f1,3,,1
wo<'isolate'>,f1.2109-f1,3,,1
wo<'issue'>,f1.2110-f1,3,,1
wo<'it'>,f1.2111-f1,3,,1
wo<'it didn',39,'t come off'>,f1.2112-f1,3,,1
wo<'it is beyond my scope'>,f1.2113-f1,3,,1
wo<'it is raining'>,f1.2114-f1,3,,1
wo<'it is snowing'>,f1.2115-f1,3,,1
wo<'it mean'>,f1.2116-f1,3,,1
wo<'it rains'>,f1.2117-f1,3,,1
wo<'it',39,'s a pity'>,f1.2118-f1,3,,1
wo<'it',39,'s my fault'>,f1.2119-f1,3,,1
wo<'item'>,f1.2120-f1,3,,1
wo<'its'>,f1.2121-f1,3,,1
wo<'ivory'>,f1.2122-f1,3,,1
wo<'ivy'>,f1.2123-f1,3,,1
wo<'jack'>,f1.2124-f1,3,,1
wo<'jackal'>,f1.2125-f1,3,,1
wo<'jacket'>,f1.2126-f1,3,,1
wo<'jar'>,f1.2127-f1,3,,1
wo<'jaw'>,f1.2128-f1,3,,1
wo<'jelly'>,f1.2129-f1,3,,1
wo<'join'>,f1.2130-f1,3,,1
wo<'joke'>,f1.2131-f1,3,,1
wo<'joking apart'>,f1.2132-f1,3,,1
wo<'journalist'>,f1.2133-f1,3,,1
wo<'judge'>,f1.2134-f1,3,,1
wo<'judgement'>,f1.2135-f1,3,,1
wo<'jug'>,f1.2136-f1,3,,1
wo<'juice'>,f1.2137-f1,3,,1
wo<'juicy'>,f1.2138-f1,3,,1
wo<'jump'>,f1.2139-f1,3,,1
wo<'jumping'>,f1.2140-f1,3,,1
wo<'just'>,f1.2141-f1,3,,1
wo<'justify'>,f1.2142-f1,3,,1
wo<'jute'>,f1.2143-f1,3,,1
wo<'juvenile'>,f1.2144-f1,3,,1
wo<'keel'>,f1.2145-f1,3,,1
wo<'keen'>,f1.2146-f1,3,,1
wo<'keep'>,f1.2147-f1,3,,1
wo<'keep in step'>,f1.2148-f1,3,,1
wo<'keep watch'>,f1.2149-f1,3,,1
wo<'keeper'>,f1.2150-f1,3,,1
wo<'kennel'>,f1.2151-f1,3,,1
wo<'kept'>,f1.2152-f1,3,,1
wo<'kerb'>,f1.2153-f1,3,,1
wo<'kill'>,f1.2154-f1,3,,1
wo<'kilogramme'>,f1.2155-f1,3,,1
wo<'kilometre'>,f1.2156-f1,3,,1
wo<'kind'>,f1.2157-f1,3,,1
wo<'king'>,f1.2158-f1,3,,1
wo<'kingdom'>,f1.2159-f1,3,,1
wo<'kitchen'>,f1.2160-f1,3,,1
wo<'kitchen-garden'>,f1.2161-f1,3,,1
wo<'knew'>,f1.2162-f1,3,,3
wo<'knight'>,f1.2163-f1,3,,1
wo<'know'>,f1.2164-f1,3,,1
wo<'knowledge'>,f1.2165-f1,3,,1
wo<'label'>,f1.2166-f1,3,,1
wo<'laboratory'>,f1.2167-f1,3,,1
wo<'labour'>,f1.2168-f1,3,,1
wo<'labour-training'>,f1.2169-f1,3,,1
wo<'lace'>,f1.2170-f1,3,,1
wo<'lack'>,f1.2171-f1,3,,1
wo<'lad'>,f1.2172-f1,3,,1
wo<'ladder'>,f1.2173-f1,3,,1
wo<'lady'>,f1.2174-f1,3,,1
wo<'ladybird'>,f1.2175-f1,3,,1
wo<'lake'>,f1.2176-f1,3,,1
wo<'lamp'>,f1.2177-f1,3,,1
wo<'land'>,f1.2178-f1,3,,1
wo<'landlady'>,f1.2179-f1,3,,1
wo<'landlord'>,f1.2180-f1,3,,1
wo<'landowner'>,f1.2181-f1,3,,1
wo<'landscape'>,f1.2182-f1,3,,1
wo<'lane'>,f1.2183-f1,3,,1
wo<'language'>,f1.2184-f1,3,,1
wo<'large'>,f1.2185-f1,3,,1
wo<'lark'>,f1.2186-f1,3,,1
wo<'lash'>,f1.2187-f1,3,,1
wo<'lass'>,f1.2188-f1,3,,1
wo<'last'>,f1.2189-f1,3,,1
wo<'last night'>,f1.2190-f1,3,,1
wo<'late'>,f1.2191-f1,3,,1
wo<'later'>,f1.2192-f1,3,,1
wo<'lathe'>,f1.2193-f1,3,,1
wo<'laugh'>,f1.2194-f1,3,,1
wo<'lawyer'>,f1.2195-f1,3,,1
wo<'lay'>,f1.2196-f1,3,,3
wo<'lay the table'>,f1.2197-f1,3,,1
wo<'laziness'>,f1.2198-f1,3,,1
wo<'lazy'>,f1.2199-f1,3,,1
wo<'lead'>,f1.2200-f1,3,,1
wo<'leader'>,f1.2201-f1,3,,1
wo<'leadership'>,f1.2202-f1,3,,1
wo<'leading article'>,f1.2203-f1,3,,1
wo<'leaf'>,f1.2204-f1,3,,1
wo<'league'>,f1.2205-f1,3,,1
wo<'leap'>,f1.2206-f1,3,,1
wo<'leaped'>,f1.2207-f1,3,,1
wo<'leapt'>,f1.2208-f1,3,,1
wo<'learn'>,f1.2209-f1,3,,1
wo<'learned'>,f1.2210-f1,3,,1
wo<'learnt'>,f1.2211-f1,3,,1
wo<'leave'>,f1.2212-f1,3,,1
wo<'led'>,f1.2213-f1,3,,1
wo<'left'>,f1.2214-f1,3,,1
wo<'leg'>,f1.2215-f1,3,,1
wo<'legend'>,f1.2216-f1,3,,1
wo<'legible'>,f1.2217-f1,3,,1
wo<'legislation'>,f1.2218-f1,3,,1
wo<'lent'>,f1.2219-f1,3,,1
wo<'less'>,f1.2220-f1,3,,1
wo<'lesson'>,f1.2221-f1,3,,1
wo<'let'>,f1.2222-f1,3,,1
wo<'let me see'>,f1.2223-f1,3,,1
wo<'letter'>,f1.2224-f1,3,,1
wo<'level'>,f1.2225-f1,3,,1
wo<'levy'>,f1.2226-f1,3,,1
wo<'liable'>,f1.2227-f1,3,,1
wo<'liad'>,f1.2228-f1,3,,1
wo<'liar'>,f1.2229-f1,3,,1
wo<'liberation'>,f1.2230-f1,3,,1
wo<'liberator'>,f1.2231-f1,3,,1
wo<'library'>,f1.2232-f1,3,,1
wo<'lie'>,f1.2233-f1,3,,1
wo<'lie in the sun'>,f1.2234-f1,3,,1
wo<'life'>,f1.2235-f1,3,,1
wo<'lift'>,f1.2236-f1,3,,1
wo<'light'>,f1.2237-f1,3,,1
wo<'like'>,f1.2238-f1,3,,1
wo<'lip'>,f1.2239-f1,3,,1
wo<'liquid ammonia'>,f1.2240-f1,3,,1
wo<'list'>,f1.2241-f1,3,,1
wo<'listen'>,f1.2242-f1,3,,1
wo<'listten'>,f1.2243-f1,3,,1
wo<'listtener'>,f1.2244-f1,3,,1
wo<'literal'>,f1.2245-f1,3,,1
wo<'literary'>,f1.2246-f1,3,,1
wo<'literate'>,f1.2247-f1,3,,1
wo<'literature'>,f1.2248-f1,3,,1
wo<'litre'>,f1.2249-f1,3,,1
wo<'litter'>,f1.2250-f1,3,,1
wo<'little'>,f1.2251-f1,3,,1
wo<'live'>,f1.2252-f1,3,,1
wo<'living-room'>,f1.2253-f1,3,,1
wo<'log'>,f1.2254-f1,3,,1
wo<'long'>,f1.2255-f1,3,,1
wo<'long ago'>,f1.2256-f1,3,,1
wo<'look'>,f1.2257-f1,3,,1
wo<'look after'>,f1.2258-f1,3,,1
wo<'look for'>,f1.2259-f1,3,,1
wo<'look like'>,f1.2260-f1,3,,1
wo<'lorry'>,f1.2261-f1,3,,1
wo<'lose'>,f1.2262-f1,3,,1
wo<'loss'>,f1.2263-f1,3,,1
wo<'lost'>,f1.2264-f1,3,,1
wo<'loud'>,f1.2265-f1,3,,1
wo<'loudly'>,f1.2266-f1,3,,1
wo<'love'>,f1.2267-f1,3,,1
wo<'low'>,f1.2268-f1,3,,1
wo<'low spirits'>,f1.2269-f1,3,,1
wo<'lower'>,f1.2270-f1,3,,1
wo<'loyal'>,f1.2271-f1,3,,1
wo<'loyalty'>,f1.2272-f1,3,,1
wo<'luggage'>,f1.2273-f1,3,,1
wo<'lull'>,f1.2274-f1,3,,1
wo<'lumber'>,f1.2275-f1,3,,1
wo<'lunch'>,f1.2276-f1,3,,1
wo<'machine'>,f1.2277-f1,3,,1
wo<'machine-gun'>,f1.2278-f1,3,,1
wo<'made'>,f1.2279-f1,3,,1
wo<'magazine'>,f1.2280-f1,3,,1
wo<'mail'>,f1.2281-f1,3,,1
wo<'main'>,f1.2282-f1,3,,1
wo<'make'>,f1.2283-f1,3,,1
wo<'make a bed'>,f1.2284-f1,3,,1
wo<'make a dialogue'>,f1.2285-f1,3,,1
wo<'make a snowman'>,f1.2286-f1,3,,1
wo<'make amends'>,f1.2287-f1,3,,1
wo<'make amends for'>,f1.2288-f1,3,,1
wo<'make faces'>,f1.2289-f1,3,,1
wo<'male'>,f1.2290-f1,3,,1
wo<'man'>,f1.2291-f1,3,,1
wo<'manage'>,f1.2292-f1,3,,1
wo<'manifesto'>,f1.2293-f1,3,,1
wo<'mankind'>,f1.2294-f1,3,,1
wo<'manner'>,f1.2295-f1,3,,1
wo<'manual'>,f1.2296-f1,3,,1
wo<'manufacture'>,f1.2297-f1,3,,1
wo<'manure'>,f1.2298-f1,3,,1
wo<'manuscript'>,f1.2299-f1,3,,1
wo<'many'>,f1.2300-f1,3,,1
wo<'map'>,f1.2301-f1,3,,1
wo<'maple'>,f1.2302-f1,3,,1
wo<'marble'>,f1.2303-f1,3,,1
wo<'march'>,f1.2304-f1,3,,1
wo<'mare'>,f1.2305-f1,3,,1
wo<'margin'>,f1.2306-f1,3,,1
wo<'marine'>,f1.2307-f1,3,,1
wo<'mark'>,f1.2308-f1,3,,1
wo<'market'>,f1.2309-f1,3,,1
wo<'marriage'>,f1.2310-f1,3,,1
wo<'married'>,f1.2311-f1,3,,1
wo<'marry'>,f1.2312-f1,3,,1
wo<'marsh'>,f1.2313-f1,3,,1
wo<'marshal'>,f1.2314-f1,3,,1
wo<'master'>,f1.2315-f1,3,,1
wo<'masterpiece'>,f1.2316-f1,3,,1
wo<'match'>,f1.2317-f1,3,,1
wo<'mathematics'>,f1.2318-f1,3,,1
wo<'mattress'>,f1.2319-f1,3,,1
wo<'mature'>,f1.2320-f1,3,,1
wo<'may'>,f1.2321-f1,3,,1
wo<'mayor'>,f1.2322-f1,3,,1
wo<'me'>,f1.2323-f1,3,,1
wo<'meadow'>,f1.2324-f1,3,,1
wo<'meal'>,f1.2325-f1,3,,1
wo<'mean'>,f1.2326-f1,3,,1
wo<'meaning'>,f1.2327-f1,3,,1
wo<'means'>,f1.2328-f1,3,,1
wo<'meant'>,f1.2329-f1,3,,1
wo<'meat'>,f1.2330-f1,3,,1
wo<'medal'>,f1.2331-f1,3,,1
wo<'meddle'>,f1.2332-f1,3,,1
wo<'medical'>,f1.2333-f1,3,,1
wo<'medicine'>,f1.2334-f1,3,,1
wo<'meek'>,f1.2335-f1,3,,1
wo<'meet'>,f1.2336-f1,3,,1
wo<'meeting'>,f1.2337-f1,3,,1
wo<'melody'>,f1.2338-f1,3,,1
wo<'melon'>,f1.2339-f1,3,,1
wo<'melt'>,f1.2340-f1,3,,1
wo<'member'>,f1.2341-f1,3,,1
wo<'membership'>,f1.2342-f1,3,,1
wo<'memorable'>,f1.2343-f1,3,,1
wo<'memory'>,f1.2344-f1,3,,1
wo<'men'>,f1.2345-f1,3,,1
wo<'mercenary'>,f1.2346-f1,3,,1
wo<'merchant'>,f1.2347-f1,3,,1
wo<'merciful'>,f1.2348-f1,3,,1
wo<'mercury'>,f1.2349-f1,3,,1
wo<'mercy'>,f1.2350-f1,3,,1
wo<'merely'>,f1.2351-f1,3,,1
wo<'meridian'>,f1.2352-f1,3,,1
wo<'merit'>,f1.2353-f1,3,,1
wo<'merry'>,f1.2354-f1,3,,1
wo<'met'>,f1.2355-f1,3,,1
wo<'metal'>,f1.2356-f1,3,,1
wo<'meteor'>,f1.2357-f1,3,,1
wo<'meter'>,f1.2358-f1,3,,1
wo<'method'>,f1.2359-f1,3,,1
wo<'metre'>,f1.2360-f1,3,,1
wo<'midday'>,f1.2361-f1,3,,1
wo<'middle'>,f1.2362-f1,3,,1
wo<'midnight'>,f1.2363-f1,3,,1
wo<'midwife'>,f1.2364-f1,3,,1
wo<'might'>,f1.2365-f1,3,,1
wo<'mighty'>,f1.2366-f1,3,,1
wo<'mile'>,f1.2367-f1,3,,1
wo<'mileage'>,f1.2368-f1,3,,1
wo<'military'>,f1.2369-f1,3,,1
wo<'milk'>,f1.2370-f1,3,,1
wo<'milkmaid'>,f1.2371-f1,3,,1
wo<'mill'>,f1.2372-f1,3,,1
wo<'miller'>,f1.2373-f1,3,,1
wo<'millet'>,f1.2374-f1,3,,1
wo<'million'>,f1.2375-f1,3,,1
wo<'millionaire'>,f1.2376-f1,3,,1
wo<'miner'>,f1.2377-f1,3,,1
wo<'mineral'>,f1.2378-f1,3,,1
wo<'minimum'>,f1.2379-f1,3,,1
wo<'minister'>,f1.2380-f1,3,,1
wo<'ministry'>,f1.2381-f1,3,,1
wo<'minor'>,f1.2382-f1,3,,1
wo<'minority'>,f1.2383-f1,3,,1
wo<'mint'>,f1.2384-f1,3,,1
wo<'minus'>,f1.2385-f1,3,,1
wo<'minute'>,f1.2386-f1,3,,1
wo<'miracle'>,f1.2387-f1,3,,1
wo<'mirror'>,f1.2388-f1,3,,1
wo<'miser'>,f1.2389-f1,3,,1
wo<'miserable'>,f1.2390-f1,3,,1
wo<'misery'>,f1.2391-f1,3,,1
wo<'misfortune'>,f1.2392-f1,3,,1
wo<'mistake'>,f1.2393-f1,3,,1
wo<'mistook'>,f1.2394-f1,3,,3
wo<'mixed'>,f1.2395-f1,3,,1
wo<'modern'>,f1.2396-f1,3,,1
wo<'modest'>,f1.2397-f1,3,,1
wo<'mole'>,f1.2398-f1,3,,1
wo<'money'>,f1.2399-f1,3,,1
wo<'money order'>,f1.2400-f1,3,,1
wo<'monkey'>,f1.2401-f1,3,,1
wo<'month'>,f1.2402-f1,3,,1
wo<'monument'>,f1.2403-f1,3,,1
wo<'mood'>,f1.2404-f1,3,,1
wo<'moon'>,f1.2405-f1,3,,1
wo<'more'>,f1.2406-f1,3,,1
wo<'morning'>,f1.2407-f1,3,,1
wo<'most'>,f1.2408-f1,3,,1
wo<'moth'>,f1.2409-f1,3,,1
wo<'mother'>,f1.2410-f1,3,,1
wo<'mother tongue'>,f1.2411-f1,3,,1
wo<'motherland'>,f1.2412-f1,3,,1
wo<'motor'>,f1.2413-f1,3,,1
wo<'motor-car'>,f1.2414-f1,3,,1
wo<'mountain'>,f1.2415-f1,3,,1
wo<'mouse'>,f1.2416-f1,3,,1
wo<'mouth'>,f1.2417-f1,3,,1
wo<'move'>,f1.2418-f1,3,,1
wo<'movement'>,f1.2419-f1,3,,1
wo<'much'>,f1.2420-f1,3,,1
wo<'museum'>,f1.2421-f1,3,,1
wo<'mushroom'>,f1.2422-f1,3,,1
wo<'music'>,f1.2423-f1,3,,1
wo<'musician'>,f1.2424-f1,3,,1
wo<'must'>,f1.2425-f1,3,,1
wo<'my'>,f1.2426-f1,3,,1
wo<'myself'>,f1.2427-f1,3,,1
wo<'name'>,f1.2428-f1,3,,1
wo<'namely'>,f1.2429-f1,3,,1
wo<'narrow'>,f1.2430-f1,3,,1
wo<'nation'>,f1.2431-f1,3,,1
wo<'nation-wide'>,f1.2432-f1,3,,1
wo<'national'>,f1.2433-f1,3,,1
wo<'national anthem'>,f1.2434-f1,3,,1
wo<'national history'>,f1.2435-f1,3,,1
wo<'native'>,f1.2436-f1,3,,1
wo<'natural'>,f1.2437-f1,3,,1
wo<'naturally'>,f1.2438-f1,3,,1
wo<'nature'>,f1.2439-f1,3,,1
wo<'near'>,f1.2440-f1,3,,1
wo<'neck'>,f1.2441-f1,3,,1
wo<'need'>,f1.2442-f1,3,,1
wo<'neighbouring'>,f1.2443-f1,3,,1
wo<'nerve'>,f1.2444-f1,3,,1
wo<'net'>,f1.2445-f1,3,,1
wo<'never'>,f1.2446-f1,3,,1
wo<'new'>,f1.2447-f1,3,,1
wo<'newcomer'>,f1.2448-f1,3,,1
wo<'newspaper'>,f1.2449-f1,3,,1
wo<'next'>,f1.2450-f1,3,,1
wo<'nib'>,f1.2451-f1,3,,1
wo<'nice'>,f1.2452-f1,3,,1
wo<'nickel'>,f1.2453-f1,3,,1
wo<'night'>,f1.2454-f1,3,,1
wo<'nightmare'>,f1.2455-f1,3,,1
wo<'nine'>,f1.2456-f1,3,,1
wo<'ninth'>,f1.2457-f1,3,,1
wo<'nitrogen'>,f1.2458-f1,3,,1
wo<'no'>,f1.2459-f1,3,,1
wo<'no admission'>,f1.2460-f1,3,,1
wo<'noble'>,f1.2461-f1,3,,1
wo<'nobleman'>,f1.2462-f1,3,,1
wo<'nobody'>,f1.2463-f1,3,,1
wo<'nod'>,f1.2464-f1,3,,1
wo<'noise'>,f1.2465-f1,3,,1
wo<'noiseless'>,f1.2466-f1,3,,1
wo<'noisy'>,f1.2467-f1,3,,1
wo<'north'>,f1.2468-f1,3,,1
wo<'nose'>,f1.2469-f1,3,,1
wo<'nostril'>,f1.2470-f1,3,,1
wo<'not'>,f1.2471-f1,3,,1
wo<'not at all'>,f1.2472-f1,3,,1
wo<'noted'>,f1.2473-f1,3,,1
wo<'nothing'>,f1.2474-f1,3,,1
wo<'notice'>,f1.2475-f1,3,,1
wo<'novel'>,f1.2476-f1,3,,1
wo<'now'>,f1.2477-f1,3,,1
wo<'nowadays'>,f1.2478-f1,3,,1
wo<'nowhere'>,f1.2479-f1,3,,1
wo<'nuclear'>,f1.2480-f1,3,,1
wo<'numb'>,f1.2481-f1,3,,1
wo<'number'>,f1.2482-f1,3,,1
wo<'nurse'>,f1.2483-f1,3,,1
wo<'nut'>,f1.2484-f1,3,,1
wo<'nutritious'>,f1.2485-f1,3,,1
wo<'oak'>,f1.2486-f1,3,,1
wo<'oaken'>,f1.2487-f1,3,,1
wo<'oar'>,f1.2488-f1,3,,1
wo<'oat'>,f1.2489-f1,3,,1
wo<'oath'>,f1.2490-f1,3,,1
wo<'oatmeal'>,f1.2491-f1,3,,1
wo<'oats'>,f1.2492-f1,3,,1
wo<'obedience'>,f1.2493-f1,3,,1
wo<'obedient'>,f1.2494-f1,3,,1
wo<'obey'>,f1.2495-f1,3,,1
wo<'object'>,f1.2496-f1,3,,1
wo<'objection'>,f1.2497-f1,3,,1
wo<'obligation'>,f1.2498-f1,3,,1
wo<'obtain'>,f1.2499-f1,3,,1
wo<'ocean'>,f1.2500-f1,3,,1
wo<'of course'>,f1.2501-f1,3,,1
wo<'off the record'>,f1.2502-f1,3,,1
wo<'office'>,f1.2503-f1,3,,1
wo<'officer'>,f1.2504-f1,3,,1
wo<'often'>,f1.2505-f1,3,,1
wo<'oil'>,f1.2506-f1,3,,1
wo<'oilcloth'>,f1.2507-f1,3,,1
wo<'oilpaint'>,f1.2508-f1,3,,1
wo<'old'>,f1.2509-f1,3,,1
wo<'on'>,f1.2510-f1,3,,1
wo<'on account of'>,f1.2511-f1,3,,1
wo<'on active service'>,f1.2512-f1,3,,1
wo<'on an average'>,f1.2513-f1,3,,1
wo<'on duty'>,f1.2514-f1,3,,1
wo<'on foot'>,f1.2515-f1,3,,1
wo<'on no account'>,f1.2516-f1,3,,1
wo<'on page'>,f1.2517-f1,3,,1
wo<'on the alert'>,f1.2518-f1,3,,1
wo<'on the eve'>,f1.2519-f1,3,,1
wo<'on the left'>,f1.2520-f1,3,,1
wo<'on the right'>,f1.2521-f1,3,,1
wo<'on the right hand of'>,f1.2522-f1,3,,1
wo<'one'>,f1.2523-f1,3,,1
wo<'onion'>,f1.2524-f1,3,,1
wo<'only'>,f1.2525-f1,3,,1
wo<'ontment'>,f1.2526-f1,3,,1
wo<'open'>,f1.2527-f1,3,,1
wo<'operate'>,f1.2528-f1,3,,1
wo<'opposite'>,f1.2529-f1,3,,1
wo<'or'>,f1.2530-f1,3,,1
wo<'oral'>,f1.2531-f1,3,,1
wo<'orchard'>,f1.2532-f1,3,,1
wo<'order'>,f1.2533-f1,3,,1
wo<'ore'>,f1.2534-f1,3,,1
wo<'organ'>,f1.2535-f1,3,,1
wo<'organic'>,f1.2536-f1,3,,1
wo<'organisation'>,f1.2537-f1,3,,1
wo<'organism'>,f1.2538-f1,3,,1
wo<'organization'>,f1.2539-f1,3,,1
wo<'organize'>,f1.2540-f1,3,,1
wo<'organized'>,f1.2541-f1,3,,3
wo<'organizer'>,f1.2542-f1,3,,1
wo<'oriental'>,f1.2543-f1,3,,1
wo<'origin'>,f1.2544-f1,3,,1
wo<'original'>,f1.2545-f1,3,,1
wo<'originate'>,f1.2546-f1,3,,1
wo<'ornament'>,f1.2547-f1,3,,1
wo<'other'>,f1.2548-f1,3,,1
wo<'other wise'>,f1.2549-f1,3,,1
wo<'our'>,f1.2550-f1,3,,1
wo<'ourselves'>,f1.2551-f1,3,,1
wo<'out'>,f1.2552-f1,3,,1
wo<'out of tune'>,f1.2553-f1,3,,1
wo<'outbreak'>,f1.2554-f1,3,,1
wo<'outcome'>,f1.2555-f1,3,,1
wo<'outcry'>,f1.2556-f1,3,,1
wo<'outer'>,f1.2557-f1,3,,1
wo<'outer space'>,f1.2558-f1,3,,1
wo<'outside'>,f1.2559-f1,3,,1
wo<'oval'>,f1.2560-f1,3,,1
wo<'oven'>,f1.2561-f1,3,,1
wo<'over'>,f1.2562-f1,3,,1
wo<'overcoat'>,f1.2563-f1,3,,1
wo<'own'>,f1.2564-f1,3,,1
wo<'own affairs'>,f1.2565-f1,3,,1
wo<'oxygen'>,f1.2566-f1,3,,1
wo<'oyster'>,f1.2567-f1,3,,1
wo<'pack'>,f1.2568-f1,3,,1
wo<'package'>,f1.2569-f1,3,,1
wo<'pact'>,f1.2570-f1,3,,1
wo<'page'>,f1.2571-f1,3,,1
wo<'paid'>,f1.2572-f1,3,,1
wo<'paint'>,f1.2573-f1,3,,1
wo<'painter'>,f1.2574-f1,3,,1
wo<'painting'>,f1.2575-f1,3,,1
wo<'pair'>,f1.2576-f1,3,,1
wo<'pal'>,f1.2577-f1,3,,1
wo<'palace'>,f1.2578-f1,3,,1
wo<'pale'>,f1.2579-f1,3,,1
wo<'palm'>,f1.2580-f1,3,,1
wo<'palmphlet'>,f1.2581-f1,3,,1
wo<'pancake'>,f1.2582-f1,3,,1
wo<'pane'>,f1.2583-f1,3,,1
wo<'paper'>,f1.2584-f1,3,,1
wo<'paper-mill'>,f1.2585-f1,3,,1
wo<'parade'>,f1.2586-f1,3,,1
wo<'paragraph'>,f1.2587-f1,3,,1
wo<'parent'>,f1.2588-f1,3,,1
wo<'parents'>,f1.2589-f1,3,,1
wo<'park'>,f1.2590-f1,3,,1
wo<'parliament'>,f1.2591-f1,3,,1
wo<'parlour'>,f1.2592-f1,3,,1
wo<'parrot'>,f1.2593-f1,3,,1
wo<'parsley'>,f1.2594-f1,3,,1
wo<'part'>,f1.2595-f1,3,,1
wo<'partial'>,f1.2596-f1,3,,1
wo<'participant'>,f1.2597-f1,3,,1
wo<'participate'>,f1.2598-f1,3,,1
wo<'participle'>,f1.2599-f1,3,,1
wo<'party'>,f1.2600-f1,3,,1
wo<'pass'>,f1.2601-f1,3,,1
wo<'passenge'>,f1.2602-f1,3,,1
wo<'passenger'>,f1.2603-f1,3,,1
wo<'passer-by'>,f1.2604-f1,3,,1
wo<'past'>,f1.2605-f1,3,,1
wo<'path'>,f1.2606-f1,3,,1
wo<'pathetic'>,f1.2607-f1,3,,1
wo<'patience'>,f1.2608-f1,3,,1
wo<'patient'>,f1.2609-f1,3,,1
wo<'patriot'>,f1.2610-f1,3,,1
wo<'patriotic'>,f1.2611-f1,3,,1
wo<'pattern'>,f1.2612-f1,3,,1
wo<'pause'>,f1.2613-f1,3,,1
wo<'pay'>,f1.2614-f1,3,,1
wo<'pea'>,f1.2615-f1,3,,1
wo<'peace'>,f1.2616-f1,3,,1
wo<'peaceful'>,f1.2617-f1,3,,1
wo<'peach'>,f1.2618-f1,3,,1
wo<'peacock'>,f1.2619-f1,3,,1
wo<'peak'>,f1.2620-f1,3,,1
wo<'peanut'>,f1.2621-f1,3,,1
wo<'pear'>,f1.2622-f1,3,,1
wo<'pearl'>,f1.2623-f1,3,,1
wo<'peasant'>,f1.2624-f1,3,,1
wo<'peat'>,f1.2625-f1,3,,1
wo<'pebble'>,f1.2626-f1,3,,1
wo<'peck'>,f1.2627-f1,3,,1
wo<'pen'>,f1.2628-f1,3,,1
wo<'pen-name'>,f1.2629-f1,3,,1
wo<'penalty'>,f1.2630-f1,3,,1
wo<'pence'>,f1.2631-f1,3,,1
wo<'pencil'>,f1.2632-f1,3,,1
wo<'pencil-box'>,f1.2633-f1,3,,1
wo<'penetrate'>,f1.2634-f1,3,,1
wo<'penholder'>,f1.2635-f1,3,,1
wo<'peninsula'>,f1.2636-f1,3,,1
wo<'pensioner'>,f1.2637-f1,3,,1
wo<'people'>,f1.2638-f1,3,,1
wo<'peoples'>,f1.2639-f1,3,,1
wo<'perfect'>,f1.2640-f1,3,,1
wo<'perform'>,f1.2641-f1,3,,1
wo<'period'>,f1.2642-f1,3,,1
wo<'perish'>,f1.2643-f1,3,,1
wo<'personal'>,f1.2644-f1,3,,1
wo<'photo'>,f1.2645-f1,3,,1
wo<'physical training'>,f1.2646-f1,3,,1
wo<'physics'>,f1.2647-f1,3,,1
wo<'piano'>,f1.2648-f1,3,,1
wo<'pick'>,f1.2649-f1,3,,1
wo<'pick up'>,f1.2650-f1,3,,1
wo<'picture'>,f1.2651-f1,3,,1
wo<'pie'>,f1.2652-f1,3,,1
wo<'piece'>,f1.2653-f1,3,,1
wo<'pig'>,f1.2654-f1,3,,1
wo<'pig-farm'>,f1.2655-f1,3,,1
wo<'pigeon'>,f1.2656-f1,3,,1
wo<'pike'>,f1.2657-f1,3,,1
wo<'pile'>,f1.2658-f1,3,,1
wo<'pilgrimage'>,f1.2659-f1,3,,1
wo<'pill'>,f1.2660-f1,3,,1
wo<'pin'>,f1.2661-f1,3,,1
wo<'pity'>,f1.2662-f1,3,,1
wo<'pivot'>,f1.2663-f1,3,,1
wo<'place'>,f1.2664-f1,3,,1
wo<'plain'>,f1.2665-f1,3,,1
wo<'plaintive'>,f1.2666-f1,3,,1
wo<'plait'>,f1.2667-f1,3,,1
wo<'plan'>,f1.2668-f1,3,,1
wo<'plane'>,f1.2669-f1,3,,1
wo<'planet'>,f1.2670-f1,3,,1
wo<'plank'>,f1.2671-f1,3,,1
wo<'plant'>,f1.2672-f1,3,,1
wo<'plantation'>,f1.2673-f1,3,,1
wo<'plaster'>,f1.2674-f1,3,,1
wo<'plate'>,f1.2675-f1,3,,1
wo<'platform'>,f1.2676-f1,3,,1
wo<'play'>,f1.2677-f1,3,,1
wo<'play chess'>,f1.2678-f1,3,,1
wo<'play snowballs'>,f1.2679-f1,3,,1
wo<'play the piano'>,f1.2680-f1,3,,1
wo<'player'>,f1.2681-f1,3,,1
wo<'playground'>,f1.2682-f1,3,,1
wo<'plead'>,f1.2683-f1,3,,1
wo<'please'>,f1.2684-f1,3,,1
wo<'pleasure'>,f1.2685-f1,3,,1
wo<'plot'>,f1.2686-f1,3,,1
wo<'plucky'>,f1.2687-f1,3,,1
wo<'plum'>,f1.2688-f1,3,,1
wo<'plump'>,f1.2689-f1,3,,1
wo<'plunder'>,f1.2690-f1,3,,1
wo<'plus'>,f1.2691-f1,3,,1
wo<'pneumonia'>,f1.2692-f1,3,,1
wo<'pocket'>,f1.2693-f1,3,,1
wo<'poem'>,f1.2694-f1,3,,1
wo<'poet'>,f1.2695-f1,3,,1
wo<'point'>,f1.2696-f1,3,,1
wo<'point of view'>,f1.2697-f1,3,,1
wo<'point out'>,f1.2698-f1,3,,1
wo<'pointed'>,f1.2699-f1,3,,1
wo<'pointer'>,f1.2700-f1,3,,1
wo<'poison'>,f1.2701-f1,3,,1
wo<'poisonous'>,f1.2702-f1,3,,1
wo<'polite'>,f1.2703-f1,3,,1
wo<'political'>,f1.2704-f1,3,,1
wo<'politician'>,f1.2705-f1,3,,1
wo<'politics'>,f1.2706-f1,3,,1
wo<'poll'>,f1.2707-f1,3,,1
wo<'pomp'>,f1.2708-f1,3,,1
wo<'pond'>,f1.2709-f1,3,,1
wo<'pony'>,f1.2710-f1,3,,1
wo<'pool'>,f1.2711-f1,3,,1
wo<'poor'>,f1.2712-f1,3,,1
wo<'pope'>,f1.2713-f1,3,,1
wo<'poplar'>,f1.2714-f1,3,,1
wo<'poppy'>,f1.2715-f1,3,,1
wo<'popular'>,f1.2716-f1,3,,1
wo<'pore'>,f1.2717-f1,3,,1
wo<'pork'>,f1.2718-f1,3,,1
wo<'porridge'>,f1.2719-f1,3,,1
wo<'port'>,f1.2720-f1,3,,1
wo<'porter'>,f1.2721-f1,3,,1
wo<'portion'>,f1.2722-f1,3,,1
wo<'portrait'>,f1.2723-f1,3,,1
wo<'position'>,f1.2724-f1,3,,1
wo<'possible'>,f1.2725-f1,3,,1
wo<'poster'>,f1.2726-f1,3,,1
wo<'postman'>,f1.2727-f1,3,,1
wo<'potato'>,f1.2728-f1,3,,1
wo<'poultry'>,f1.2729-f1,3,,1
wo<'poultry farm'>,f1.2730-f1,3,,1
wo<'powder'>,f1.2731-f1,3,,1
wo<'power'>,f1.2732-f1,3,,1
wo<'powerful'>,f1.2733-f1,3,,1
wo<'powor of attorney'>,f1.2734-f1,3,,1
wo<'practically'>,f1.2735-f1,3,,1
wo<'preach'>,f1.2736-f1,3,,1
wo<'precarious'>,f1.2737-f1,3,,1
wo<'precaution'>,f1.2738-f1,3,,1
wo<'precede'>,f1.2739-f1,3,,1
wo<'precedent'>,f1.2740-f1,3,,1
wo<'preceding'>,f1.2741-f1,3,,1
wo<'precious'>,f1.2742-f1,3,,1
wo<'precipice'>,f1.2743-f1,3,,1
wo<'precise'>,f1.2744-f1,3,,1
wo<'precision'>,f1.2745-f1,3,,1
wo<'predecessor'>,f1.2746-f1,3,,1
wo<'prefer'>,f1.2747-f1,3,,1
wo<'prepare'>,f1.2748-f1,3,,1
wo<'prescription'>,f1.2749-f1,3,,1
wo<'presence'>,f1.2750-f1,3,,1
wo<'present'>,f1.2751-f1,3,,1
wo<'pressed'>,f1.2752-f1,3,,1
wo<'pretty'>,f1.2753-f1,3,,1
wo<'prevalent'>,f1.2754-f1,3,,1
wo<'prevent'>,f1.2755-f1,3,,1
wo<'prevention'>,f1.2756-f1,3,,1
wo<'previous'>,f1.2757-f1,3,,1
wo<'previous to'>,f1.2758-f1,3,,1
wo<'price'>,f1.2759-f1,3,,1
wo<'priceless'>,f1.2760-f1,3,,1
wo<'prick'>,f1.2761-f1,3,,1
wo<'prickly'>,f1.2762-f1,3,,1
wo<'pride'>,f1.2763-f1,3,,1
wo<'priest'>,f1.2764-f1,3,,1
wo<'primary'>,f1.2765-f1,3,,1
wo<'private'>,f1.2766-f1,3,,1
wo<'prize'>,f1.2767-f1,3,,1
wo<'probability'>,f1.2768-f1,3,,1
wo<'probably'>,f1.2769-f1,3,,1
wo<'problem'>,f1.2770-f1,3,,1
wo<'proceed'>,f1.2771-f1,3,,1
wo<'proceeding'>,f1.2772-f1,3,,1
wo<'procees'>,f1.2773-f1,3,,1
wo<'procession'>,f1.2774-f1,3,,1
wo<'proclaim'>,f1.2775-f1,3,,1
wo<'proclamation'>,f1.2776-f1,3,,1
wo<'procure'>,f1.2777-f1,3,,1
wo<'produce'>,f1.2778-f1,3,,1
wo<'producer'>,f1.2779-f1,3,,1
wo<'product'>,f1.2780-f1,3,,1
wo<'production'>,f1.2781-f1,3,,1
wo<'profession'>,f1.2782-f1,3,,1
wo<'professional'>,f1.2783-f1,3,,1
wo<'professor'>,f1.2784-f1,3,,1
wo<'profit'>,f1.2785-f1,3,,1
wo<'profitable'>,f1.2786-f1,3,,1
wo<'profound'>,f1.2787-f1,3,,1
wo<'program'>,f1.2788-f1,3,,1
wo<'programme'>,f1.2789-f1,3,,1
wo<'programming'>,f1.2790-f1,3,,1
wo<'progress'>,f1.2791-f1,3,,1
wo<'progressive'>,f1.2792-f1,3,,1
wo<'promise'>,f1.2793-f1,3,,1
wo<'prop'>,f1.2794-f1,3,,1
wo<'propaganda'>,f1.2795-f1,3,,1
wo<'proper'>,f1.2796-f1,3,,1
wo<'proper name'>,f1.2797-f1,3,,1
wo<'proper noum'>,f1.2798-f1,3,,1
wo<'properly'>,f1.2799-f1,3,,1
wo<'property'>,f1.2800-f1,3,,1
wo<'proud'>,f1.2801-f1,3,,1
wo<'prove'>,f1.2802-f1,3,,1
wo<'proverb'>,f1.2803-f1,3,,1
wo<'provide'>,f1.2804-f1,3,,1
wo<'provided'>,f1.2805-f1,3,,1
wo<'providence'>,f1.2806-f1,3,,1
wo<'province'>,f1.2807-f1,3,,1
wo<'provocation'>,f1.2808-f1,3,,1
wo<'provocative'>,f1.2809-f1,3,,1
wo<'provoke'>,f1.2810-f1,3,,1
wo<'prudent'>,f1.2811-f1,3,,1
wo<'pub'>,f1.2812-f1,3,,1
wo<'public'>,f1.2813-f1,3,,1
wo<'publication'>,f1.2814-f1,3,,1
wo<'publicity'>,f1.2815-f1,3,,1
wo<'publish'>,f1.2816-f1,3,,1
wo<'pudding'>,f1.2817-f1,3,,1
wo<'pull'>,f1.2818-f1,3,,1
wo<'pupil'>,f1.2819-f1,3,,1
wo<'puppet'>,f1.2820-f1,3,,1
wo<'puppy'>,f1.2821-f1,3,,1
wo<'purchase'>,f1.2822-f1,3,,1
wo<'pure'>,f1.2823-f1,3,,1
wo<'pure imagination'>,f1.2824-f1,3,,1
wo<'purge'>,f1.2825-f1,3,,1
wo<'purity'>,f1.2826-f1,3,,1
wo<'purple'>,f1.2827-f1,3,,1
wo<'purse'>,f1.2828-f1,3,,1
wo<'pursue'>,f1.2829-f1,3,,1
wo<'put'>,f1.2830-f1,3,,1
wo<'put down'>,f1.2831-f1,3,,1
wo<'put in'>,f1.2832-f1,3,,1
wo<'put off'>,f1.2833-f1,3,,1
wo<'put on'>,f1.2834-f1,3,,1
wo<'put out'>,f1.2835-f1,3,,1
wo<'qualification'>,f1.2836-f1,3,,1
wo<'quality'>,f1.2837-f1,3,,1
wo<'quantity'>,f1.2838-f1,3,,1
wo<'quarrel'>,f1.2839-f1,3,,1
wo<'quarry'>,f1.2840-f1,3,,1
wo<'quarter'>,f1.2841-f1,3,,1
wo<'quay'>,f1.2842-f1,3,,1
wo<'queen'>,f1.2843-f1,3,,1
wo<'queer'>,f1.2844-f1,3,,1
wo<'quench'>,f1.2845-f1,3,,1
wo<'quest'>,f1.2846-f1,3,,1
wo<'question'>,f1.2847-f1,3,,1
wo<'questionnaire'>,f1.2848-f1,3,,1
wo<'queue'>,f1.2849-f1,3,,1
wo<'quick'>,f1.2850-f1,3,,1
wo<'quickly'>,f1.2851-f1,3,,1
wo<'quicksilver'>,f1.2852-f1,3,,1
wo<'quiet'>,f1.2853-f1,3,,1
wo<'quiet down'>,f1.2854-f1,3,,1
wo<'quilt'>,f1.2855-f1,3,,1
wo<'quit'>,f1.2856-f1,3,,1
wo<'quite'>,f1.2857-f1,3,,1
wo<'quiver'>,f1.2858-f1,3,,1
wo<'quotation'>,f1.2859-f1,3,,1
wo<'quotation-marks'>,f1.2860-f1,3,,1
wo<'quote'>,f1.2861-f1,3,,1
wo<'rabbit'>,f1.2862-f1,3,,1
wo<'race'>,f1.2863-f1,3,,1
wo<'racial'>,f1.2864-f1,3,,1
wo<'radiant'>,f1.2865-f1,3,,1
wo<'radiator'>,f1.2866-f1,3,,1
wo<'radical'>,f1.2867-f1,3,,1
wo<'radio'>,f1.2868-f1,3,,1
wo<'radish'>,f1.2869-f1,3,,1
wo<'raft'>,f1.2870-f1,3,,1
wo<'rag'>,f1.2871-f1,3,,1
wo<'rage'>,f1.2872-f1,3,,1
wo<'ragged'>,f1.2873-f1,3,,1
wo<'raid'>,f1.2874-f1,3,,1
wo<'rail'>,f1.2875-f1,3,,1
wo<'railing'>,f1.2876-f1,3,,1
wo<'railroad'>,f1.2877-f1,3,,1
wo<'railway'>,f1.2878-f1,3,,1
wo<'railway station'>,f1.2879-f1,3,,1
wo<'railwayman'>,f1.2880-f1,3,,1
wo<'rain'>,f1.2881-f1,3,,1
wo<'rainbow'>,f1.2882-f1,3,,1
wo<'raincoat'>,f1.2883-f1,3,,1
wo<'rainy'>,f1.2884-f1,3,,1
wo<'raise'>,f1.2885-f1,3,,1
wo<'raise one',39,'s hopes'>,f1.2886-f1,3,,1
wo<'raise one',39,'s voice'>,f1.2887-f1,3,,1
wo<'raisin'>,f1.2888-f1,3,,1
wo<'rake'>,f1.2889-f1,3,,1
wo<'ram'>,f1.2890-f1,3,,1
wo<'ran'>,f1.2891-f1,3,,3
wo<'rang'>,f1.2892-f1,3,,3
wo<'rat'>,f1.2893-f1,3,,1
wo<'ration'>,f1.2894-f1,3,,1
wo<'rational'>,f1.2895-f1,3,,1
wo<'ray'>,f1.2896-f1,3,,1
wo<'reach'>,f1.2897-f1,3,,1
wo<'read'>,f1.2898-f1,3,,1
wo<'reader'>,f1.2899-f1,3,,1
wo<'readily'>,f1.2900-f1,3,,1
wo<'reading'>,f1.2901-f1,3,,1
wo<'reading-room'>,f1.2902-f1,3,,1
wo<'ready'>,f1.2903-f1,3,,1
wo<'real'>,f1.2904-f1,3,,1
wo<'realize'>,f1.2905-f1,3,,1
wo<'really'>,f1.2906-f1,3,,1
wo<'recall'>,f1.2907-f1,3,,1
wo<'receipt'>,f1.2908-f1,3,,1
wo<'receive'>,f1.2909-f1,3,,1
wo<'receiver'>,f1.2910-f1,3,,1
wo<'recent'>,f1.2911-f1,3,,1
wo<'recently'>,f1.2912-f1,3,,1
wo<'reception'>,f1.2913-f1,3,,1
wo<'recite'>,f1.2914-f1,3,,1
wo<'reckless'>,f1.2915-f1,3,,1
wo<'reckon'>,f1.2916-f1,3,,1
wo<'reckon on'>,f1.2917-f1,3,,1
wo<'recognition'>,f1.2918-f1,3,,1
wo<'recognize'>,f1.2919-f1,3,,1
wo<'recollect'>,f1.2920-f1,3,,1
wo<'recollection'>,f1.2921-f1,3,,1
wo<'recommend'>,f1.2922-f1,3,,1
wo<'recommendation'>,f1.2923-f1,3,,1
wo<'recompense'>,f1.2924-f1,3,,1
wo<'reconcile'>,f1.2925-f1,3,,1
wo<'reconnaissance'>,f1.2926-f1,3,,1
wo<'reconstruct'>,f1.2927-f1,3,,1
wo<'reconstruction'>,f1.2928-f1,3,,1
wo<'record'>,f1.2929-f1,3,,1
wo<'record of service'>,f1.2930-f1,3,,1
wo<'recover'>,f1.2931-f1,3,,1
wo<'recovery'>,f1.2932-f1,3,,1
wo<'recruit'>,f1.2933-f1,3,,1
wo<'recur'>,f1.2934-f1,3,,1
wo<'red'>,f1.2935-f1,3,,1
wo<'redeem'>,f1.2936-f1,3,,1
wo<'reduce'>,f1.2937-f1,3,,1
wo<'reduction'>,f1.2938-f1,3,,1
wo<'refer'>,f1.2939-f1,3,,1
wo<'reference'>,f1.2940-f1,3,,1
wo<'reflect'>,f1.2941-f1,3,,1
wo<'reflection'>,f1.2942-f1,3,,1
wo<'reform'>,f1.2943-f1,3,,1
wo<'refrain'>,f1.2944-f1,3,,1
wo<'refrain from'>,f1.2945-f1,3,,1
wo<'refresh'>,f1.2946-f1,3,,1
wo<'refreshment'>,f1.2947-f1,3,,1
wo<'refreshment room'>,f1.2948-f1,3,,1
wo<'refrigerator'>,f1.2949-f1,3,,1
wo<'region'>,f1.2950-f1,3,,1
wo<'reins'>,f1.2951-f1,3,,1
wo<'reiterate'>,f1.2952-f1,3,,1
wo<'reject'>,f1.2953-f1,3,,1
wo<'rejoice'>,f1.2954-f1,3,,1
wo<'relate'>,f1.2955-f1,3,,1
wo<'relation'>,f1.2956-f1,3,,1
wo<'relationship'>,f1.2957-f1,3,,1
wo<'relative'>,f1.2958-f1,3,,1
wo<'relax'>,f1.2959-f1,3,,1
wo<'release'>,f1.2960-f1,3,,1
wo<'relent'>,f1.2961-f1,3,,1
wo<'relentless'>,f1.2962-f1,3,,1
wo<'reliable'>,f1.2963-f1,3,,1
wo<'reliance'>,f1.2964-f1,3,,1
wo<'relief'>,f1.2965-f1,3,,1
wo<'religion'>,f1.2966-f1,3,,1
wo<'religious'>,f1.2967-f1,3,,1
wo<'relive'>,f1.2968-f1,3,,1
wo<'reluctant'>,f1.2969-f1,3,,1
wo<'reluctantly'>,f1.2970-f1,3,,1
wo<'rely'>,f1.2971-f1,3,,1
wo<'rely upon'>,f1.2972-f1,3,,1
wo<'remain'>,f1.2973-f1,3,,1
wo<'remainder'>,f1.2974-f1,3,,1
wo<'remark'>,f1.2975-f1,3,,1
wo<'remarkable'>,f1.2976-f1,3,,1
wo<'remedy'>,f1.2977-f1,3,,1
wo<'remember'>,f1.2978-f1,3,,1
wo<'remembrance'>,f1.2979-f1,3,,1
wo<'remind'>,f1.2980-f1,3,,1
wo<'reminiscence'>,f1.2981-f1,3,,1
wo<'remnant'>,f1.2982-f1,3,,1
wo<'remorse'>,f1.2983-f1,3,,1
wo<'remote'>,f1.2984-f1,3,,1
wo<'removal'>,f1.2985-f1,3,,1
wo<'remove'>,f1.2986-f1,3,,1
wo<'renew'>,f1.2987-f1,3,,1
wo<'rent'>,f1.2988-f1,3,,1
wo<'repeat'>,f1.2989-f1,3,,1
wo<'replace'>,f1.2990-f1,3,,1
wo<'reply'>,f1.2991-f1,3,,1
wo<'report'>,f1.2992-f1,3,,1
wo<'reporter'>,f1.2993-f1,3,,1
wo<'republic'>,f1.2994-f1,3,,1
wo<'reputation'>,f1.2995-f1,3,,1
wo<'request'>,f1.2996-f1,3,,1
wo<'require'>,f1.2997-f1,3,,1
wo<'rescue'>,f1.2998-f1,3,,1
wo<'research'>,f1.2999-f1,3,,1
wo<'resound'>,f1.3000-f1,3,,1
wo<'respect'>,f1.3001-f1,3,,1
wo<'respectable'>,f1.3002-f1,3,,1
wo<'respectful'>,f1.3003-f1,3,,1
wo<'respective'>,f1.3004-f1,3,,1
wo<'respite'>,f1.3005-f1,3,,1
wo<'respond'>,f1.3006-f1,3,,1
wo<'response'>,f1.3007-f1,3,,1
wo<'responsibility'>,f1.3008-f1,3,,1
wo<'rest'>,f1.3009-f1,3,,1
wo<'result'>,f1.3010-f1,3,,1
wo<'revolution'>,f1.3011-f1,3,,1
wo<'rib'>,f1.3012-f1,3,,1
wo<'ribbon'>,f1.3013-f1,3,,1
wo<'rich'>,f1.3014-f1,3,,1
wo<'riches'>,f1.3015-f1,3,,1
wo<'ride'>,f1.3016-f1,3,,1
wo<'right'>,f1.3017-f1,3,,1
wo<'ring'>,f1.3018-f1,3,,1
wo<'ripe'>,f1.3019-f1,3,,1
wo<'rise'>,f1.3020-f1,3,,1
wo<'risk'>,f1.3021-f1,3,,1
wo<'rival'>,f1.3022-f1,3,,1
wo<'river'>,f1.3023-f1,3,,1
wo<'rlent'>,f1.3024-f1,3,,1
wo<'rlentless'>,f1.3025-f1,3,,1
wo<'road'>,f1.3026-f1,3,,1
wo<'roam'>,f1.3027-f1,3,,1
wo<'roar'>,f1.3028-f1,3,,1
wo<'roast'>,f1.3029-f1,3,,1
wo<'rob'>,f1.3030-f1,3,,1
wo<'robber'>,f1.3031-f1,3,,1
wo<'robbery'>,f1.3032-f1,3,,1
wo<'robin'>,f1.3033-f1,3,,1
wo<'rock'>,f1.3034-f1,3,,1
wo<'rode'>,f1.3035-f1,3,,3
wo<'role'>,f1.3036-f1,3,,1
wo<'roll'>,f1.3037-f1,3,,1
wo<'roll-call'>,f1.3038-f1,3,,1
wo<'roller'>,f1.3039-f1,3,,1
wo<'romance'>,f1.3040-f1,3,,1
wo<'romantic'>,f1.3041-f1,3,,1
wo<'roof'>,f1.3042-f1,3,,1
wo<'rook'>,f1.3043-f1,3,,1
wo<'room'>,f1.3044-f1,3,,1
wo<'rope'>,f1.3045-f1,3,,1
wo<'rose'>,f1.3046-f1,3,,1
wo<'rosy'>,f1.3047-f1,3,,1
wo<'round'>,f1.3048-f1,3,,1
wo<'rouse'>,f1.3049-f1,3,,1
wo<'route'>,f1.3050-f1,3,,1
wo<'row'>,f1.3051-f1,3,,1
wo<'ruin'>,f1.3052-f1,3,,1
wo<'ruler'>,f1.3053-f1,3,,1
wo<'run'>,f1.3054-f1,3,,1
wo<'run away'>,f1.3055-f1,3,,1
wo<'runner'>,f1.3056-f1,3,,1
wo<'running'>,f1.3057-f1,3,,1
wo<'sable'>,f1.3058-f1,3,,1
wo<'sack'>,f1.3059-f1,3,,1
wo<'sacred'>,f1.3060-f1,3,,1
wo<'sacrifice'>,f1.3061-f1,3,,1
wo<'sad'>,f1.3062-f1,3,,1
wo<'saddle'>,f1.3063-f1,3,,1
wo<'safe'>,f1.3064-f1,3,,1
wo<'safe and sound'>,f1.3065-f1,3,,1
wo<'safeguard'>,f1.3066-f1,3,,1
wo<'safety'>,f1.3067-f1,3,,1
wo<'said'>,f1.3068-f1,3,,1
wo<'sail'>,f1.3069-f1,3,,1
wo<'sailor'>,f1.3070-f1,3,,1
wo<'saint'>,f1.3071-f1,3,,1
wo<'sake'>,f1.3072-f1,3,,1
wo<'salad'>,f1.3073-f1,3,,1
wo<'salary'>,f1.3074-f1,3,,1
wo<'sale'>,f1.3075-f1,3,,1
wo<'salesman'>,f1.3076-f1,3,,1
wo<'salmon'>,f1.3077-f1,3,,1
wo<'salt'>,f1.3078-f1,3,,1
wo<'salute'>,f1.3079-f1,3,,1
wo<'same'>,f1.3080-f1,3,,1
wo<'sample'>,f1.3081-f1,3,,1
wo<'sanction'>,f1.3082-f1,3,,1
wo<'sand'>,f1.3083-f1,3,,1
wo<'sandwich'>,f1.3084-f1,3,,1
wo<'sane'>,f1.3085-f1,3,,1
wo<'sang'>,f1.3086-f1,3,,3
wo<'sanitary'>,f1.3087-f1,3,,1
wo<'sank'>,f1.3088-f1,3,,3
wo<'sat'>,f1.3089-f1,3,,1
wo<'save'>,f1.3090-f1,3,,1
wo<'saw'>,f1.3091-f1,3,,3
wo<'say'>,f1.3092-f1,3,,1
wo<'scarf'>,f1.3093-f1,3,,1
wo<'school'>,f1.3094-f1,3,,1
wo<'school uniform'>,f1.3095-f1,3,,1
wo<'school-leaver'>,f1.3096-f1,3,,1
wo<'schoolboy'>,f1.3097-f1,3,,1
wo<'schoolchildren'>,f1.3098-f1,3,,1
wo<'schoolgirl'>,f1.3099-f1,3,,1
wo<'schoolyard'>,f1.3100-f1,3,,1
wo<'science'>,f1.3101-f1,3,,1
wo<'scientist'>,f1.3102-f1,3,,1
wo<'scope'>,f1.3103-f1,3,,1
wo<'score'>,f1.3104-f1,3,,1
wo<'scout'>,f1.3105-f1,3,,1
wo<'screen'>,f1.3106-f1,3,,1
wo<'sculptor'>,f1.3107-f1,3,,1
wo<'sea'>,f1.3108-f1,3,,1
wo<'sea-calf'>,f1.3109-f1,3,,1
wo<'sea-gull'>,f1.3110-f1,3,,1
wo<'seal'>,f1.3111-f1,3,,1
wo<'seam'>,f1.3112-f1,3,,1
wo<'seaside'>,f1.3113-f1,3,,1
wo<'season'>,f1.3114-f1,3,,1
wo<'seat'>,f1.3115-f1,3,,1
wo<'second'>,f1.3116-f1,3,,1
wo<'secondary'>,f1.3117-f1,3,,1
wo<'secret'>,f1.3118-f1,3,,1
wo<'section'>,f1.3119-f1,3,,1
wo<'see'>,f1.3120-f1,3,,1
wo<'see off'>,f1.3121-f1,3,,1
wo<'seed'>,f1.3122-f1,3,,1
wo<'seldom'>,f1.3123-f1,3,,1
wo<'select'>,f1.3124-f1,3,,1
wo<'selection'>,f1.3125-f1,3,,1
wo<'sell'>,f1.3126-f1,3,,1
wo<'send'>,f1.3127-f1,3,,1
wo<'senior'>,f1.3128-f1,3,,1
wo<'senior pupil'>,f1.3129-f1,3,,1
wo<'sensation'>,f1.3130-f1,3,,1
wo<'sense'>,f1.3131-f1,3,,1
wo<'senseless'>,f1.3132-f1,3,,1
wo<'sent'>,f1.3133-f1,3,,1
wo<'sentence'>,f1.3134-f1,3,,1
wo<'serious'>,f1.3135-f1,3,,1
wo<'serve'>,f1.3136-f1,3,,1
wo<'serving men'>,f1.3137-f1,3,,1
wo<'session'>,f1.3138-f1,3,,1
wo<'set'>,f1.3139-f1,3,,1
wo<'seven'>,f1.3140-f1,3,,1
wo<'seventh'>,f1.3141-f1,3,,1
wo<'several'>,f1.3142-f1,3,,1
wo<'she'>,f1.3143-f1,3,,1
wo<'sheep'>,f1.3144-f1,3,,1
wo<'sheep-dog'>,f1.3145-f1,3,,1
wo<'sheet'>,f1.3146-f1,3,,1
wo<'shelf'>,f1.3147-f1,3,,1
wo<'shine'>,f1.3148-f1,3,,1
wo<'ship'>,f1.3149-f1,3,,1
wo<'shipment'>,f1.3150-f1,3,,1
wo<'shipwreck'>,f1.3151-f1,3,,1
wo<'shipyard'>,f1.3152-f1,3,,1
wo<'shirt'>,f1.3153-f1,3,,1
wo<'shoe'>,f1.3154-f1,3,,1
wo<'shone'>,f1.3155-f1,3,,1
wo<'shoot'>,f1.3156-f1,3,,1
wo<'shop'>,f1.3157-f1,3,,1
wo<'shop-assistant'>,f1.3158-f1,3,,1
wo<'shopping'>,f1.3159-f1,3,,1
wo<'shopping-bag'>,f1.3160-f1,3,,1
wo<'shore'>,f1.3161-f1,3,,1
wo<'short'>,f1.3162-f1,3,,1
wo<'shot'>,f1.3163-f1,3,,1
wo<'shovel'>,f1.3164-f1,3,,1
wo<'show'>,f1.3165-f1,3,,1
wo<'showed'>,f1.3166-f1,3,,3
wo<'shrewd'>,f1.3167-f1,3,,1
wo<'shriek'>,f1.3168-f1,3,,1
wo<'shrill'>,f1.3169-f1,3,,1
wo<'shrink'>,f1.3170-f1,3,,1
wo<'shut'>,f1.3171-f1,3,,1
wo<'sickness'>,f1.3172-f1,3,,1
wo<'side'>,f1.3173-f1,3,,1
wo<'side by'>,f1.3174-f1,3,,1
wo<'sidewalk'>,f1.3175-f1,3,,1
wo<'sideways'>,f1.3176-f1,3,,1
wo<'siege'>,f1.3177-f1,3,,1
wo<'sieve'>,f1.3178-f1,3,,1
wo<'sigh'>,f1.3179-f1,3,,1
wo<'sight'>,f1.3180-f1,3,,1
wo<'sights'>,f1.3181-f1,3,,1
wo<'sightseeing'>,f1.3182-f1,3,,1
wo<'sign'>,f1.3183-f1,3,,1
wo<'signal'>,f1.3184-f1,3,,1
wo<'signature'>,f1.3185-f1,3,,1
wo<'signboard'>,f1.3186-f1,3,,1
wo<'significance'>,f1.3187-f1,3,,1
wo<'significant'>,f1.3188-f1,3,,1
wo<'signify'>,f1.3189-f1,3,,1
wo<'silence'>,f1.3190-f1,3,,1
wo<'silent'>,f1.3191-f1,3,,1
wo<'silk'>,f1.3192-f1,3,,1
wo<'sill'>,f1.3193-f1,3,,1
wo<'silly'>,f1.3194-f1,3,,1
wo<'silver'>,f1.3195-f1,3,,1
wo<'simple'>,f1.3196-f1,3,,1
wo<'simultaneous'>,f1.3197-f1,3,,1
wo<'sin'>,f1.3198-f1,3,,1
wo<'since'>,f1.3199-f1,3,,1
wo<'sing'>,f1.3200-f1,3,,1
wo<'singer'>,f1.3201-f1,3,,1
wo<'single'>,f1.3202-f1,3,,1
wo<'sir'>,f1.3203-f1,3,,1
wo<'sister'>,f1.3204-f1,3,,1
wo<'sit'>,f1.3205-f1,3,,1
wo<'situated'>,f1.3206-f1,3,,1
wo<'six'>,f1.3207-f1,3,,1
wo<'sixth'>,f1.3208-f1,3,,1
wo<'size'>,f1.3209-f1,3,,1
wo<'skate'>,f1.3210-f1,3,,1
wo<'ski'>,f1.3211-f1,3,,1
wo<'skin'>,f1.3212-f1,3,,1
wo<'skin-daving'>,f1.3213-f1,3,,1
wo<'skirt'>,f1.3214-f1,3,,1
wo<'skull'>,f1.3215-f1,3,,1
wo<'sky'>,f1.3216-f1,3,,1
wo<'sledge'>,f1.3217-f1,3,,1
wo<'sleep'>,f1.3218-f1,3,,1
wo<'slept'>,f1.3219-f1,3,,1
wo<'slice'>,f1.3220-f1,3,,1
wo<'slow'>,f1.3221-f1,3,,1
wo<'slowly'>,f1.3222-f1,3,,1
wo<'small'>,f1.3223-f1,3,,1
wo<'smile'>,f1.3224-f1,3,,1
wo<'smoke'>,f1.3225-f1,3,,1
wo<'snow'>,f1.3226-f1,3,,1
wo<'snowball'>,f1.3227-f1,3,,1
wo<'snowman'>,f1.3228-f1,3,,1
wo<'so'>,f1.3229-f1,3,,1
wo<'soap'>,f1.3230-f1,3,,1
wo<'socialist system'>,f1.3231-f1,3,,1
wo<'sock'>,f1.3232-f1,3,,1
wo<'sofa'>,f1.3233-f1,3,,1
wo<'sold'>,f1.3234-f1,3,,1
wo<'solder'>,f1.3235-f1,3,,1
wo<'soldier'>,f1.3236-f1,3,,1
wo<'sole'>,f1.3237-f1,3,,1
wo<'solemn'>,f1.3238-f1,3,,1
wo<'solid'>,f1.3239-f1,3,,1
wo<'some'>,f1.3240-f1,3,,1
wo<'somebody'>,f1.3241-f1,3,,1
wo<'something'>,f1.3242-f1,3,,1
wo<'sometimes'>,f1.3243-f1,3,,1
wo<'son'>,f1.3244-f1,3,,1
wo<'song'>,f1.3245-f1,3,,1
wo<'soon'>,f1.3246-f1,3,,1
wo<'soot'>,f1.3247-f1,3,,1
wo<'soothe'>,f1.3248-f1,3,,1
wo<'sore'>,f1.3249-f1,3,,1
wo<'sorry'>,f1.3250-f1,3,,1
wo<'sorry, I wasn',39,'t attending'>,f1.3251-f1,3,,1
wo<'soul'>,f1.3252-f1,3,,1
wo<'sound'>,f1.3253-f1,3,,1
wo<'soup'>,f1.3254-f1,3,,1
wo<'sour'>,f1.3255-f1,3,,1
wo<'source'>,f1.3256-f1,3,,1
wo<'south'>,f1.3257-f1,3,,1
wo<'south-east'>,f1.3258-f1,3,,1
wo<'southern'>,f1.3259-f1,3,,1
wo<'souvenir'>,f1.3260-f1,3,,1
wo<'souvenirs'>,f1.3261-f1,3,,1
wo<'sovereignty'>,f1.3262-f1,3,,1
wo<'space'>,f1.3263-f1,3,,1
wo<'spaceman'>,f1.3264-f1,3,,1
wo<'spaceship'>,f1.3265-f1,3,,1
wo<'speak'>,f1.3266-f1,3,,1
wo<'speaker'>,f1.3267-f1,3,,1
wo<'special'>,f1.3268-f1,3,,1
wo<'speed'>,f1.3269-f1,3,,1
wo<'spelled'>,f1.3270-f1,3,,1
wo<'spelt'>,f1.3271-f1,3,,1
wo<'spend'>,f1.3272-f1,3,,1
wo<'spent'>,f1.3273-f1,3,,1
wo<'spoke'>,f1.3274-f1,3,,3
wo<'spoon'>,f1.3275-f1,3,,1
wo<'sport'>,f1.3276-f1,3,,1
wo<'sports ground'>,f1.3277-f1,3,,1
wo<'sportsman'>,f1.3278-f1,3,,1
wo<'spot'>,f1.3279-f1,3,,1
wo<'spotless'>,f1.3280-f1,3,,1
wo<'spout'>,f1.3281-f1,3,,1
wo<'sprang'>,f1.3282-f1,3,,3
wo<'spring'>,f1.3283-f1,3,,1
wo<'square'>,f1.3284-f1,3,,1
wo<'stability'>,f1.3285-f1,3,,1
wo<'stable'>,f1.3286-f1,3,,1
wo<'stack'>,f1.3287-f1,3,,1
wo<'stadium'>,f1.3288-f1,3,,1
wo<'staff'>,f1.3289-f1,3,,1
wo<'stage'>,f1.3290-f1,3,,1
wo<'stagger'>,f1.3291-f1,3,,1
wo<'stagnation'>,f1.3292-f1,3,,1
wo<'stalemate'>,f1.3293-f1,3,,1
wo<'stalk'>,f1.3294-f1,3,,1
wo<'stamp'>,f1.3295-f1,3,,1
wo<'stand'>,f1.3296-f1,3,,1
wo<'stand up'>,f1.3297-f1,3,,1
wo<'star'>,f1.3298-f1,3,,1
wo<'stare'>,f1.3299-f1,3,,1
wo<'starling'>,f1.3300-f1,3,,1
wo<'start'>,f1.3301-f1,3,,1
wo<'startle'>,f1.3302-f1,3,,1
wo<'starvation'>,f1.3303-f1,3,,1
wo<'starve'>,f1.3304-f1,3,,1
wo<'state'>,f1.3305-f1,3,,1
wo<'station'>,f1.3306-f1,3,,1
wo<'statue'>,f1.3307-f1,3,,1
wo<'stay'>,f1.3308-f1,3,,1
wo<'stay behind'>,f1.3309-f1,3,,1
wo<'stay-at-home'>,f1.3310-f1,3,,1
wo<'steel'>,f1.3311-f1,3,,1
wo<'steep'>,f1.3312-f1,3,,1
wo<'steer'>,f1.3313-f1,3,,1
wo<'stellar'>,f1.3314-f1,3,,1
wo<'stem'>,f1.3315-f1,3,,1
wo<'step'>,f1.3316-f1,3,,1
wo<'stewardess'>,f1.3317-f1,3,,1
wo<'stick'>,f1.3318-f1,3,,1
wo<'still'>,f1.3319-f1,3,,1
wo<'still better'>,f1.3320-f1,3,,1
wo<'still waters run deep'>,f1.3321-f1,3,,1
wo<'stock'>,f1.3322-f1,3,,1
wo<'stock exchange'>,f1.3323-f1,3,,1
wo<'stocking'>,f1.3324-f1,3,,1
wo<'stone'>,f1.3325-f1,3,,1
wo<'stood'>,f1.3326-f1,3,,1
wo<'stop'>,f1.3327-f1,3,,1
wo<'stopper'>,f1.3328-f1,3,,1
wo<'storage'>,f1.3329-f1,3,,1
wo<'store'>,f1.3330-f1,3,,1
wo<'storey'>,f1.3331-f1,3,,1
wo<'stork'>,f1.3332-f1,3,,1
wo<'storm'>,f1.3333-f1,3,,1
wo<'stormy'>,f1.3334-f1,3,,1
wo<'story'>,f1.3335-f1,3,,1
wo<'stout'>,f1.3336-f1,3,,1
wo<'stove'>,f1.3337-f1,3,,1
wo<'straight'>,f1.3338-f1,3,,1
wo<'straighten'>,f1.3339-f1,3,,1
wo<'strain'>,f1.3340-f1,3,,1
wo<'strait'>,f1.3341-f1,3,,1
wo<'strange'>,f1.3342-f1,3,,1
wo<'street'>,f1.3343-f1,3,,1
wo<'strike'>,f1.3344-f1,3,,1
wo<'strip'>,f1.3345-f1,3,,1
wo<'stripe'>,f1.3346-f1,3,,1
wo<'striped'>,f1.3347-f1,3,,1
wo<'strive'>,f1.3348-f1,3,,1
wo<'stroke'>,f1.3349-f1,3,,1
wo<'stroll'>,f1.3350-f1,3,,1
wo<'strong'>,f1.3351-f1,3,,1
wo<'struck'>,f1.3352-f1,3,,1
wo<'struggle'>,f1.3353-f1,3,,1
wo<'student'>,f1.3354-f1,3,,1
wo<'study'>,f1.3355-f1,3,,1
wo<'stuff'>,f1.3356-f1,3,,1
wo<'stumble'>,f1.3357-f1,3,,1
wo<'stumbling-block'>,f1.3358-f1,3,,1
wo<'stump'>,f1.3359-f1,3,,1
wo<'stun'>,f1.3360-f1,3,,1
wo<'stupid'>,f1.3361-f1,3,,1
wo<'style'>,f1.3362-f1,3,,1
wo<'subdue'>,f1.3363-f1,3,,1
wo<'subdued'>,f1.3364-f1,3,,1
wo<'subject'>,f1.3365-f1,3,,1
wo<'submit'>,f1.3366-f1,3,,1
wo<'subtle'>,f1.3367-f1,3,,1
wo<'subtlety'>,f1.3368-f1,3,,1
wo<'subtract'>,f1.3369-f1,3,,1
wo<'subtraction'>,f1.3370-f1,3,,1
wo<'suburb'>,f1.3371-f1,3,,1
wo<'suburban'>,f1.3372-f1,3,,1
wo<'subway'>,f1.3373-f1,3,,1
wo<'success'>,f1.3374-f1,3,,1
wo<'successful'>,f1.3375-f1,3,,1
wo<'such'>,f1.3376-f1,3,,1
wo<'suck'>,f1.3377-f1,3,,1
wo<'suddenly'>,f1.3378-f1,3,,1
wo<'sugar'>,f1.3379-f1,3,,1
wo<'suit'>,f1.3380-f1,3,,1
wo<'sum'>,f1.3381-f1,3,,1
wo<'sum up'>,f1.3382-f1,3,,1
wo<'summer'>,f1.3383-f1,3,,1
wo<'sunny'>,f1.3384-f1,3,,1
wo<'superfluous'>,f1.3385-f1,3,,1
wo<'supper'>,f1.3386-f1,3,,1
wo<'supplement'>,f1.3387-f1,3,,1
wo<'supply'>,f1.3388-f1,3,,1
wo<'support'>,f1.3389-f1,3,,1
wo<'supporter'>,f1.3390-f1,3,,1
wo<'suppose'>,f1.3391-f1,3,,1
wo<'sure'>,f1.3392-f1,3,,1
wo<'surface'>,f1.3393-f1,3,,1
wo<'surgeon'>,f1.3394-f1,3,,1
wo<'surgery'>,f1.3395-f1,3,,1
wo<'surname'>,f1.3396-f1,3,,1
wo<'surpass'>,f1.3397-f1,3,,1
wo<'surplus'>,f1.3398-f1,3,,1
wo<'surplus value'>,f1.3399-f1,3,,1
wo<'swallow'>,f1.3400-f1,3,,1
wo<'swam'>,f1.3401-f1,3,,3
wo<'sweep'>,f1.3402-f1,3,,1
wo<'sweetheart'>,f1.3403-f1,3,,1
wo<'swept'>,f1.3404-f1,3,,1
wo<'swim'>,f1.3405-f1,3,,1
wo<'swimming'>,f1.3406-f1,3,,1
wo<'swimming-pool'>,f1.3407-f1,3,,1
wo<'sword'>,f1.3408-f1,3,,1
wo<'symbol'>,f1.3409-f1,3,,1
wo<'sympathize'>,f1.3410-f1,3,,1
wo<'sympathy'>,f1.3411-f1,3,,1
wo<'symphony'>,f1.3412-f1,3,,1
wo<'symptom'>,f1.3413-f1,3,,1
wo<'synchrophasotron'>,f1.3414-f1,3,,1
wo<'syringe'>,f1.3415-f1,3,,1
wo<'system'>,f1.3416-f1,3,,1
wo<'table'>,f1.3417-f1,3,,1
wo<'table-cloth'>,f1.3418-f1,3,,1
wo<'tacit'>,f1.3419-f1,3,,1
wo<'tacit agreement'>,f1.3420-f1,3,,1
wo<'tact'>,f1.3421-f1,3,,1
wo<'tactful'>,f1.3422-f1,3,,1
wo<'tactless'>,f1.3423-f1,3,,1
wo<'tail'>,f1.3424-f1,3,,1
wo<'tailor'>,f1.3425-f1,3,,1
wo<'take'>,f1.3426-f1,3,,1
wo<'take apart'>,f1.3427-f1,3,,1
wo<'take care of'>,f1.3428-f1,3,,1
wo<'take cover'>,f1.3429-f1,3,,1
wo<'take off'>,f1.3430-f1,3,,1
wo<'take part'>,f1.3431-f1,3,,1
wo<'take the floor'>,f1.3432-f1,3,,1
wo<'tale'>,f1.3433-f1,3,,1
wo<'talk'>,f1.3434-f1,3,,1
wo<'tall'>,f1.3435-f1,3,,1
wo<'tape'>,f1.3436-f1,3,,1
wo<'tape-recorder'>,f1.3437-f1,3,,1
wo<'target'>,f1.3438-f1,3,,1
wo<'taught'>,f1.3439-f1,3,,1
wo<'tax'>,f1.3440-f1,3,,1
wo<'taxi'>,f1.3441-f1,3,,1
wo<'tea'>,f1.3442-f1,3,,1
wo<'tea-spoon'>,f1.3443-f1,3,,1
wo<'teach'>,f1.3444-f1,3,,1
wo<'teacher'>,f1.3445-f1,3,,1
wo<'team'>,f1.3446-f1,3,,1
wo<'tease'>,f1.3447-f1,3,,1
wo<'technical'>,f1.3448-f1,3,,1
wo<'technical drawing'>,f1.3449-f1,3,,1
wo<'technique'>,f1.3450-f1,3,,1
wo<'tedious'>,f1.3451-f1,3,,1
wo<'teem'>,f1.3452-f1,3,,1
wo<'telegram'>,f1.3453-f1,3,,1
wo<'telegraph'>,f1.3454-f1,3,,1
wo<'telephone'>,f1.3455-f1,3,,1
wo<'television'>,f1.3456-f1,3,,1
wo<'tell'>,f1.3457-f1,3,,1
wo<'tell jokes'>,f1.3458-f1,3,,1
wo<'temperate'>,f1.3459-f1,3,,1
wo<'temperature'>,f1.3460-f1,3,,1
wo<'temporary'>,f1.3461-f1,3,,1
wo<'tempt'>,f1.3462-f1,3,,1
wo<'temptation'>,f1.3463-f1,3,,1
wo<'ten'>,f1.3464-f1,3,,1
wo<'tennis'>,f1.3465-f1,3,,1
wo<'tennis player'>,f1.3466-f1,3,,1
wo<'tent'>,f1.3467-f1,3,,1
wo<'tenth'>,f1.3468-f1,3,,1
wo<'terrace'>,f1.3469-f1,3,,1
wo<'terrible'>,f1.3470-f1,3,,1
wo<'territory'>,f1.3471-f1,3,,1
wo<'test'>,f1.3472-f1,3,,1
wo<'text'>,f1.3473-f1,3,,1
wo<'text box'>,f1.3474-f1,3,,1
wo<'textile'>,f1.3475-f1,3,,1
wo<'thank'>,f1.3476-f1,3,,1
wo<'thank you'>,f1.3477-f1,3,,1
wo<'thanks'>,f1.3478-f1,3,,1
wo<'that'>,f1.3479-f1,3,,1
wo<'that',39,'s all right'>,f1.3480-f1,3,,1
wo<'that',39,'s right'>,f1.3481-f1,3,,1
wo<'that',39,'s why'>,f1.3482-f1,3,,1
wo<'the British'>,f1.3483-f1,3,,1
wo<'the match ended in a draw'>,f1.3484-f1,3,,1
wo<'the other day'>,f1.3485-f1,3,,1
wo<'the point is'>,f1.3486-f1,3,,1
wo<'the reins of goverment'>,f1.3487-f1,3,,1
wo<'the rescue'>,f1.3488-f1,3,,1
wo<'the same to you'>,f1.3489-f1,3,,1
wo<'the tube'>,f1.3490-f1,3,,1
wo<'the very'>,f1.3491-f1,3,,1
wo<'theatre'>,f1.3492-f1,3,,1
wo<'their'>,f1.3493-f1,3,,1
wo<'them'>,f1.3494-f1,3,,1
wo<'themselves'>,f1.3495-f1,3,,1
wo<'then'>,f1.3496-f1,3,,1
wo<'theory'>,f1.3497-f1,3,,1
wo<'there'>,f1.3498-f1,3,,1
wo<'there was no alternative'>,f1.3499-f1,3,,1
wo<'these'>,f1.3500-f1,3,,1
wo<'they'>,f1.3501-f1,3,,1
wo<'they are very much alike'>,f1.3502-f1,3,,1
wo<'they dress alike'>,f1.3503-f1,3,,1
wo<'thick'>,f1.3504-f1,3,,1
wo<'thief'>,f1.3505-f1,3,,1
wo<'thin'>,f1.3506-f1,3,,1
wo<'think'>,f1.3507-f1,3,,1
wo<'thinker'>,f1.3508-f1,3,,1
wo<'third'>,f1.3509-f1,3,,1
wo<'thirteen'>,f1.3510-f1,3,,1
wo<'thirteenth'>,f1.3511-f1,3,,1
wo<'thirtieth'>,f1.3512-f1,3,,1
wo<'thirty'>,f1.3513-f1,3,,1
wo<'this'>,f1.3514-f1,3,,1
wo<'those'>,f1.3515-f1,3,,1
wo<'thought'>,f1.3516-f1,3,,1
wo<'thousand'>,f1.3517-f1,3,,1
wo<'threw'>,f1.3518-f1,3,,1
wo<'throat'>,f1.3519-f1,3,,1
wo<'throw'>,f1.3520-f1,3,,1
wo<'throw away'>,f1.3521-f1,3,,1
wo<'ticket'>,f1.3522-f1,3,,1
wo<'tidhts'>,f1.3523-f1,3,,1
wo<'tidy'>,f1.3524-f1,3,,1
wo<'tie'>,f1.3525-f1,3,,1
wo<'tiger'>,f1.3526-f1,3,,1
wo<'tight'>,f1.3527-f1,3,,1
wo<'tighten'>,f1.3528-f1,3,,1
wo<'tile'>,f1.3529-f1,3,,1
wo<'till'>,f1.3530-f1,3,,1
wo<'time'>,f1.3531-f1,3,,1
wo<'time-table'>,f1.3532-f1,3,,1
wo<'tip'>,f1.3533-f1,3,,1
wo<'title'>,f1.3534-f1,3,,1
wo<'to'>,f1.3535-f1,3,,1
wo<'today'>,f1.3536-f1,3,,1
wo<'toe'>,f1.3537-f1,3,,1
wo<'together'>,f1.3538-f1,3,,1
wo<'toil'>,f1.3539-f1,3,,1
wo<'toilet'>,f1.3540-f1,3,,1
wo<'token'>,f1.3541-f1,3,,1
wo<'told'>,f1.3542-f1,3,,3
wo<'tolerable'>,f1.3543-f1,3,,1
wo<'tolerant'>,f1.3544-f1,3,,1
wo<'tomato'>,f1.3545-f1,3,,1
wo<'tomb'>,f1.3546-f1,3,,1
wo<'tomorrow'>,f1.3547-f1,3,,1
wo<'tongue'>,f1.3548-f1,3,,1
wo<'too'>,f1.3549-f1,3,,1
wo<'took'>,f1.3550-f1,3,,3
wo<'took the floor'>,f1.3551-f1,3,,1
wo<'tool'>,f1.3552-f1,3,,1
wo<'tools'>,f1.3553-f1,3,,1
wo<'top'>,f1.3554-f1,3,,1
wo<'toward'>,f1.3555-f1,3,,1
wo<'towards'>,f1.3556-f1,3,,1
wo<'towel'>,f1.3557-f1,3,,1
wo<'tower'>,f1.3558-f1,3,,1
wo<'town'>,f1.3559-f1,3,,1
wo<'toy'>,f1.3560-f1,3,,1
wo<'trace'>,f1.3561-f1,3,,1
wo<'track'>,f1.3562-f1,3,,1
wo<'tractor'>,f1.3563-f1,3,,1
wo<'tractor-driver'>,f1.3564-f1,3,,1
wo<'trade'>,f1.3565-f1,3,,1
wo<'trade union'>,f1.3566-f1,3,,1
wo<'tradition'>,f1.3567-f1,3,,1
wo<'traffic'>,f1.3568-f1,3,,1
wo<'tragedy'>,f1.3569-f1,3,,1
wo<'tragic'>,f1.3570-f1,3,,1
wo<'train'>,f1.3571-f1,3,,1
wo<'trainer'>,f1.3572-f1,3,,1
wo<'tram'>,f1.3573-f1,3,,1
wo<'translate'>,f1.3574-f1,3,,1
wo<'translation'>,f1.3575-f1,3,,1
wo<'transmission'>,f1.3576-f1,3,,1
wo<'transparent'>,f1.3577-f1,3,,1
wo<'transport'>,f1.3578-f1,3,,1
wo<'trap'>,f1.3579-f1,3,,1
wo<'trapdoor'>,f1.3580-f1,3,,1
wo<'travel'>,f1.3581-f1,3,,1
wo<'traveller'>,f1.3582-f1,3,,1
wo<'traverse'>,f1.3583-f1,3,,1
wo<'tray'>,f1.3584-f1,3,,1
wo<'treaty'>,f1.3585-f1,3,,1
wo<'tree'>,f1.3586-f1,3,,1
wo<'trip'>,f1.3587-f1,3,,1
wo<'trolley-bus'>,f1.3588-f1,3,,1
wo<'troop'>,f1.3589-f1,3,,1
wo<'troops'>,f1.3590-f1,3,,1
wo<'trousers'>,f1.3591-f1,3,,1
wo<'truck'>,f1.3592-f1,3,,1
wo<'trust'>,f1.3593-f1,3,,1
wo<'truth'>,f1.3594-f1,3,,1
wo<'try'>,f1.3595-f1,3,,1
wo<'try on'>,f1.3596-f1,3,,1
wo<'tub'>,f1.3597-f1,3,,1
wo<'tube'>,f1.3598-f1,3,,1
wo<'tulip'>,f1.3599-f1,3,,1
wo<'tune'>,f1.3600-f1,3,,1
wo<'tunnel'>,f1.3601-f1,3,,1
wo<'turf'>,f1.3602-f1,3,,1
wo<'turn'>,f1.3603-f1,3,,1
wo<'turn off'>,f1.3604-f1,3,,1
wo<'turn on'>,f1.3605-f1,3,,1
wo<'turned-up'>,f1.3606-f1,3,,1
wo<'turner'>,f1.3607-f1,3,,1
wo<'twelfth'>,f1.3608-f1,3,,1
wo<'twelve'>,f1.3609-f1,3,,1
wo<'twentieth'>,f1.3610-f1,3,,1
wo<'twenty'>,f1.3611-f1,3,,1
wo<'twenty-first'>,f1.3612-f1,3,,1
wo<'twenty-one'>,f1.3613-f1,3,,1
wo<'twice'>,f1.3614-f1,3,,1
wo<'two'>,f1.3615-f1,3,,1
wo<'tyrkey'>,f1.3616-f1,3,,1
wo<'ugly'>,f1.3617-f1,3,,1
wo<'ultimate'>,f1.3618-f1,3,,1
wo<'umbrella'>,f1.3619-f1,3,,1
wo<'uncertainty'>,f1.3620-f1,3,,1
wo<'uncle'>,f1.3621-f1,3,,1
wo<'uncomfortable'>,f1.3622-f1,3,,1
wo<'uncommon'>,f1.3623-f1,3,,1
wo<'under'>,f1.3624-f1,3,,1
wo<'underground'>,f1.3625-f1,3,,1
wo<'underline'>,f1.3626-f1,3,,1
wo<'underlined'>,f1.3627-f1,3,,1
wo<'understand'>,f1.3628-f1,3,,1
wo<'understood'>,f1.3629-f1,3,,1
wo<'uniform'>,f1.3630-f1,3,,1
wo<'union'>,f1.3631-f1,3,,1
wo<'unit'>,f1.3632-f1,3,,1
wo<'unite'>,f1.3633-f1,3,,1
wo<'unity'>,f1.3634-f1,3,,1
wo<'universal acclaim'>,f1.3635-f1,3,,1
wo<'university'>,f1.3636-f1,3,,1
wo<'unknown'>,f1.3637-f1,3,,1
wo<'unrest'>,f1.3638-f1,3,,1
wo<'up'>,f1.3639-f1,3,,1
wo<'up and down'>,f1.3640-f1,3,,1
wo<'upbringing'>,f1.3641-f1,3,,1
wo<'upper'>,f1.3642-f1,3,,1
wo<'uproar'>,f1.3643-f1,3,,1
wo<'uproot'>,f1.3644-f1,3,,1
wo<'upward'>,f1.3645-f1,3,,1
wo<'upwards'>,f1.3646-f1,3,,1
wo<'urgent'>,f1.3647-f1,3,,1
wo<'urn'>,f1.3648-f1,3,,1
wo<'us'>,f1.3649-f1,3,,1
wo<'usage'>,f1.3650-f1,3,,1
wo<'use'>,f1.3651-f1,3,,1
wo<'useful'>,f1.3652-f1,3,,1
wo<'useless'>,f1.3653-f1,3,,1
wo<'usher'>,f1.3654-f1,3,,1
wo<'usual'>,f1.3655-f1,3,,1
wo<'usually'>,f1.3656-f1,3,,1
wo<'utilize'>,f1.3657-f1,3,,1
wo<'utterly'>,f1.3658-f1,3,,1
wo<'vacancy'>,f1.3659-f1,3,,1
wo<'vacant'>,f1.3660-f1,3,,1
wo<'vacation'>,f1.3661-f1,3,,1
wo<'vacuum cleaner'>,f1.3662-f1,3,,1
wo<'vague'>,f1.3663-f1,3,,1
wo<'vain'>,f1.3664-f1,3,,1
wo<'valid'>,f1.3665-f1,3,,1
wo<'valley'>,f1.3666-f1,3,,1
wo<'valuable'>,f1.3667-f1,3,,1
wo<'value'>,f1.3668-f1,3,,1
wo<'valve'>,f1.3669-f1,3,,1
wo<'van'>,f1.3670-f1,3,,1
wo<'vanguard'>,f1.3671-f1,3,,1
wo<'vanish'>,f1.3672-f1,3,,1
wo<'vanity'>,f1.3673-f1,3,,1
wo<'vanquish'>,f1.3674-f1,3,,1
wo<'vapour'>,f1.3675-f1,3,,1
wo<'variety'>,f1.3676-f1,3,,1
wo<'variety theatre'>,f1.3677-f1,3,,1
wo<'various'>,f1.3678-f1,3,,1
wo<'varnish'>,f1.3679-f1,3,,1
wo<'vary'>,f1.3680-f1,3,,1
wo<'vase'>,f1.3681-f1,3,,1
wo<'vast'>,f1.3682-f1,3,,1
wo<'veal'>,f1.3683-f1,3,,1
wo<'vegetable'>,f1.3684-f1,3,,1
wo<'vehement'>,f1.3685-f1,3,,1
wo<'vehicle'>,f1.3686-f1,3,,1
wo<'veil'>,f1.3687-f1,3,,1
wo<'vein'>,f1.3688-f1,3,,1
wo<'velvet'>,f1.3689-f1,3,,1
wo<'vengeance'>,f1.3690-f1,3,,1
wo<'ventilate'>,f1.3691-f1,3,,1
wo<'venture'>,f1.3692-f1,3,,1
wo<'verb'>,f1.3693-f1,3,,1
wo<'verbal'>,f1.3694-f1,3,,1
wo<'verdict'>,f1.3695-f1,3,,1
wo<'verdure'>,f1.3696-f1,3,,1
wo<'verge'>,f1.3697-f1,3,,1
wo<'verge on'>,f1.3698-f1,3,,1
wo<'verify'>,f1.3699-f1,3,,1
wo<'verse'>,f1.3700-f1,3,,1
wo<'vertical'>,f1.3701-f1,3,,1
wo<'very'>,f1.3702-f1,3,,1
wo<'very much'>,f1.3703-f1,3,,1
wo<'very well'>,f1.3704-f1,3,,1
wo<'vessel'>,f1.3705-f1,3,,1
wo<'vest'>,f1.3706-f1,3,,1
wo<'veteran'>,f1.3707-f1,3,,1
wo<'vex'>,f1.3708-f1,3,,1
wo<'vexation'>,f1.3709-f1,3,,1
wo<'vibrate'>,f1.3710-f1,3,,1
wo<'vice'>,f1.3711-f1,3,,1
wo<'vice-'>,f1.3712-f1,1,,1
wo<'vicinity'>,f1.3713-f1,3,,1
wo<'vicious'>,f1.3714-f1,3,,1
wo<'victim'>,f1.3715-f1,3,,1
wo<'victorious'>,f1.3716-f1,3,,1
wo<'victory'>,f1.3717-f1,3,,1
wo<'victuals'>,f1.3718-f1,3,,1
wo<'view'>,f1.3719-f1,3,,1
wo<'vigilance'>,f1.3720-f1,3,,1
wo<'vigorous'>,f1.3721-f1,3,,1
wo<'vile'>,f1.3722-f1,3,,1
wo<'village'>,f1.3723-f1,3,,1
wo<'villager'>,f1.3724-f1,3,,1
wo<'villain'>,f1.3725-f1,3,,1
wo<'vindicate'>,f1.3726-f1,3,,1
wo<'vine'>,f1.3727-f1,3,,1
wo<'vinegar'>,f1.3728-f1,3,,1
wo<'vineyard'>,f1.3729-f1,3,,1
wo<'violate'>,f1.3730-f1,3,,1
wo<'violence'>,f1.3731-f1,3,,1
wo<'violent'>,f1.3732-f1,3,,1
wo<'violet'>,f1.3733-f1,3,,1
wo<'violin'>,f1.3734-f1,3,,1
wo<'viper'>,f1.3735-f1,3,,1
wo<'virtue'>,f1.3736-f1,3,,1
wo<'visible'>,f1.3737-f1,3,,1
wo<'vision'>,f1.3738-f1,3,,1
wo<'visit'>,f1.3739-f1,3,,1
wo<'visitor'>,f1.3740-f1,3,,1
wo<'visual'>,f1.3741-f1,3,,1
wo<'vital'>,f1.3742-f1,3,,1
wo<'vivid'>,f1.3743-f1,3,,1
wo<'vocabulary'>,f1.3744-f1,3,,1
wo<'vocal'>,f1.3745-f1,3,,1
wo<'vocation'>,f1.3746-f1,3,,1
wo<'vocational'>,f1.3747-f1,3,,1
wo<'vocational school'>,f1.3748-f1,3,,1
wo<'voice'>,f1.3749-f1,3,,1
wo<'void'>,f1.3750-f1,3,,1
wo<'volcano'>,f1.3751-f1,3,,1
wo<'volley-ball'>,f1.3752-f1,3,,1
wo<'volley-ball player'>,f1.3753-f1,3,,1
wo<'volume'>,f1.3754-f1,3,,1
wo<'voyage'>,f1.3755-f1,3,,1
wo<'wait'>,f1.3756-f1,3,,1
wo<'waiter'>,f1.3757-f1,3,,1
wo<'waiton'>,f1.3758-f1,3,,1
wo<'waitress'>,f1.3759-f1,3,,1
wo<'wake'>,f1.3760-f1,3,,1
wo<'waked'>,f1.3761-f1,3,,1
wo<'walk'>,f1.3762-f1,3,,1
wo<'wall'>,f1.3763-f1,3,,1
wo<'want'>,f1.3764-f1,3,,1
wo<'war'>,f1.3765-f1,3,,1
wo<'wardrobe'>,f1.3766-f1,3,,1
wo<'warm'>,f1.3767-f1,3,,1
wo<'warm welcome'>,f1.3768-f1,3,,1
wo<'warrant'>,f1.3769-f1,3,,1
wo<'warrior'>,f1.3770-f1,3,,1
wo<'warship'>,f1.3771-f1,3,,1
wo<'was'>,f1.3772-f1,3,,1
wo<'wash'>,f1.3773-f1,3,,1
wo<'wash up'>,f1.3774-f1,3,,1
wo<'watch'>,f1.3775-f1,3,,1
wo<'watch TV'>,f1.3776-f1,3,,1
wo<'watchful'>,f1.3777-f1,3,,1
wo<'watchman'>,f1.3778-f1,3,,1
wo<'water'>,f1.3779-f1,3,,1
wo<'water-colour'>,f1.3780-f1,3,,1
wo<'water-melon'>,f1.3781-f1,3,,1
wo<'waterfall'>,f1.3782-f1,3,,1
wo<'watering-place'>,f1.3783-f1,3,,1
wo<'watering-pot'>,f1.3784-f1,3,,1
wo<'waterproof'>,f1.3785-f1,3,,1
wo<'wave'>,f1.3786-f1,3,,1
wo<'wax'>,f1.3787-f1,3,,1
wo<'way'>,f1.3788-f1,3,,1
wo<'we'>,f1.3789-f1,3,,1
wo<'we must face facts'>,f1.3790-f1,3,,1
wo<'wealth'>,f1.3791-f1,3,,1
wo<'wear'>,f1.3792-f1,3,,1
wo<'weather'>,f1.3793-f1,3,,1
wo<'week'>,f1.3794-f1,3,,1
wo<'week-end'>,f1.3795-f1,3,,1
wo<'weekly'>,f1.3796-f1,3,,1
wo<'weep'>,f1.3797-f1,3,,1
wo<'weigh'>,f1.3798-f1,3,,1
wo<'weight'>,f1.3799-f1,3,,1
wo<'weighty'>,f1.3800-f1,3,,1
wo<'welcome'>,f1.3801-f1,3,,1
wo<'welfare'>,f1.3802-f1,3,,1
wo<'well'>,f1.3803-f1,3,,1
wo<'well off'>,f1.3804-f1,3,,1
wo<'well-read'>,f1.3805-f1,3,,1
wo<'well-to-do'>,f1.3806-f1,3,,1
wo<'went'>,f1.3807-f1,3,,3
wo<'were'>,f1.3808-f1,3,,1
wo<'west'>,f1.3809-f1,3,,1
wo<'western'>,f1.3810-f1,3,,1
wo<'wet'>,f1.3811-f1,3,,1
wo<'whale'>,f1.3812-f1,3,,1
wo<'what'>,f1.3813-f1,3,,1
wo<'what d',39,'you mean'>,f1.3814-f1,3,,1
wo<'what-ever'>,f1.3815-f1,3,,1
wo<'wheat'>,f1.3816-f1,3,,1
wo<'wheel'>,f1.3817-f1,3,,1
wo<'wheelbarrow'>,f1.3818-f1,3,,1
wo<'when'>,f1.3819-f1,3,,1
wo<'whenever'>,f1.3820-f1,3,,1
wo<'where'>,f1.3821-f1,3,,1
wo<'whereas'>,f1.3822-f1,3,,1
wo<'wherever'>,f1.3823-f1,3,,1
wo<'whether'>,f1.3824-f1,3,,1
wo<'which'>,f1.3825-f1,3,,1
wo<'which of you'>,f1.3826-f1,3,,1
wo<'while'>,f1.3827-f1,3,,1
wo<'whim'>,f1.3828-f1,3,,1
wo<'whine'>,f1.3829-f1,3,,1
wo<'whip'>,f1.3830-f1,3,,1
wo<'whirlwind'>,f1.3831-f1,3,,1
wo<'whiskers'>,f1.3832-f1,3,,1
wo<'white'>,f1.3833-f1,3,,1
wo<'who'>,f1.3834-f1,3,,1
wo<'whole'>,f1.3835-f1,3,,1
wo<'whose'>,f1.3836-f1,3,,1
wo<'why'>,f1.3837-f1,3,,1
wo<'wide'>,f1.3838-f1,3,,1
wo<'widow'>,f1.3839-f1,3,,1
wo<'widower'>,f1.3840-f1,3,,1
wo<'width'>,f1.3841-f1,3,,1
wo<'wife'>,f1.3842-f1,3,,1
wo<'wig'>,f1.3843-f1,3,,1
wo<'wild'>,f1.3844-f1,3,,1
wo<'wild flower'>,f1.3845-f1,3,,1
wo<'wilderness'>,f1.3846-f1,3,,1
wo<'win'>,f1.3847-f1,3,,1
wo<'wind'>,f1.3848-f1,3,,1
wo<'window'>,f1.3849-f1,3,,1
wo<'window-pane'>,f1.3850-f1,3,,1
wo<'windy'>,f1.3851-f1,3,,1
wo<'wine'>,f1.3852-f1,3,,1
wo<'wineglass'>,f1.3853-f1,3,,1
wo<'winter'>,f1.3854-f1,3,,1
wo<'wireless'>,f1.3855-f1,3,,1
wo<'wisdom'>,f1.3856-f1,3,,1
wo<'wise'>,f1.3857-f1,3,,1
wo<'wish'>,f1.3858-f1,3,,1
wo<'with'>,f1.3859-f1,3,,1
wo<'with reference to'>,f1.3860-f1,3,,1
wo<'within'>,f1.3861-f1,3,,1
wo<'without'>,f1.3862-f1,3,,1
wo<'woke'>,f1.3863-f1,3,,1
wo<'woman'>,f1.3864-f1,3,,1
wo<'women'>,f1.3865-f1,3,,1
wo<'won'>,f1.3866-f1,3,,1
wo<'wonder'>,f1.3867-f1,3,,1
wo<'wonderful'>,f1.3868-f1,3,,1
wo<'wood'>,f1.3869-f1,3,,1
wo<'wooden'>,f1.3870-f1,3,,1
wo<'woodpecker'>,f1.3871-f1,3,,1
wo<'woodwork'>,f1.3872-f1,3,,1
wo<'wool'>,f1.3873-f1,3,,1
wo<'woollen'>,f1.3874-f1,3,,1
wo<'word'>,f1.3875-f1,3,,1
wo<'word-combination'>,f1.3876-f1,3,,1
wo<'wore'>,f1.3877-f1,3,,3
wo<'work'>,f1.3878-f1,3,,1
wo<'worker'>,f1.3879-f1,3,,1
wo<'working people'>,f1.3880-f1,3,,1
wo<'workshop'>,f1.3881-f1,3,,1
wo<'world'>,f1.3882-f1,3,,1
wo<'worse'>,f1.3883-f1,3,,1
wo<'worst'>,f1.3884-f1,3,,1
wo<'wound'>,f1.3885-f1,3,,1
wo<'wounded'>,f1.3886-f1,3,,1
wo<'write'>,f1.3887-f1,3,,1
wo<'write down'>,f1.3888-f1,3,,1
wo<'writer'>,f1.3889-f1,3,,1
wo<'wrong'>,f1.3890-f1,3,,1
wo<'wrote'>,f1.3891-f1,3,,3
wo<'xylophone'>,f1.3892-f1,3,,1
wo<'yacht'>,f1.3893-f1,3,,1
wo<'yard'>,f1.3894-f1,3,,1
wo<'yard-keeper'>,f1.3895-f1,3,,1
wo<'yawn'>,f1.3896-f1,3,,1
wo<'year'>,f1.3897-f1,3,,1
wo<'yeast'>,f1.3898-f1,3,,1
wo<'yell'>,f1.3899-f1,3,,1
wo<'yellow'>,f1.3900-f1,3,,1
wo<'yes'>,f1.3901-f1,3,,1
wo<'yesterday'>,f1.3902-f1,3,,1
wo<'yet'>,f1.3903-f1,3,,1
wo<'yield'>,f1.3904-f1,3,,1
wo<'yoke'>,f1.3905-f1,3,,1
wo<'yolk'>,f1.3906-f1,3,,1
wo<'you'>,f1.3907-f1,3,,1
wo<'young'>,f1.3908-f1,3,,1
wo<'your'>,f1.3909-f1,3,,1
wo<'yourself'>,f1.3910-f1,3,,1
wo<'yourselves'>,f1.3911-f1,3,,1
wo<'youth'>,f1.3912-f1,3,,1
wo<'zeal'>,f1.3913-f1,3,,1
wo<'zenith'>,f1.3914-f1,3,,1
wo<'zero'>,f1.3915-f1,3,,1
wo<'zink'>,f1.3916-f1,3,,1
wo<'zip-fastener'>,f1.3917-f1,3,,1
wo<'zipper'>,f1.3918-f1,3,,1
wo<'zone'>,f1.3919-f1,3,,1
wo<'zoo'>,f1.3920-f1,3,,1
wo<'zoology'>,f1.3921-f1,3,,1
f1: db 0
.12:db '��䠢��, ���㪠, �㪢���, ���⪨, �᭮��',0
.13:db '��������',0
.14:db '����',0
.15:db '�����-�����',0
.16:db '�䣠�᪨�, �䣠���',0
.17:db '�䣠���⠭',0
.18:db '��ਪ�',0
.19:db '��ਪ��᪨�, ��ਪ����',0
.20:db '�������',0
.21:db '�����᪨� ���஢�',0
.22:db '���᪠',0
.23:db '�������',0
.24:db '�����᪨�, �������',0
.25:db '����� (��࠭�)',0
.26:db '����� (��த)',0
.27:db '����',0
.28:db '�. ��������',0
.29:db '���ਪ�',0
.30:db '���ਪ��᪨�, ���ਪ����',0
.31:db '�����',0
.32:db '����ठ�',0
.33:db '�����',0
.34:db '����⨪�',0
.35:db '��������',0
.36:db '��५�',0
.37:db '��⨪�',0
.38:db '������ �������� �����',0
.39:db '�ࣥ�⨭�',0
.40:db '�ਧ���',0
.41:db '�ઠ���� (��� � ��த)',0
.42:db '����',0
.43:db '�����᪨�',0
.44:db '�䨭�',0
.45:db '�⫠���᪨� �����',0
.46:db '������',0
.47:db '����ࠫ��',0
.48:db '����ࠫ��᪨�, ����ࠫ��',0
.49:db '������',0
.50:db '����ࠤ',0
.51:db '��⠭᪨�, ������᪨�',0
.52:db '����ᥫ�',0
.53:db '������',0
.54:db '����',0
.55:db '�������',0
.56:db '�����୨�',0
.57:db '����ਤ�',0
.58:db '������',0
.59:db '�������',0
.60:db '����ࡥ�',0
.61:db '������',0
.62:db '��ઠ���',0
.63:db '��୨���',0
.64:db '��୮���',0
.65:db '����',0
.66:db '��⠩',0
.67:db '��⠩᪨�, ��⠥�',0
.68:db '஦���⢮, ஦���⢥���',0
.69:db '�������',0
.70:db '������',0
.71:db '����������',0
.72:db '��न����',0
.73:db '���㮫�',0
.74:db '�㡠',0
.75:db '�������',0
.76:db '��������᪠� ���㡫��� ��⭠�',0
.77:db '����ய��஢�',0
.78:db '�����',0
.79:db '�������᪨�',0
.80:db '���������',0
.81:db '����',0
.82:db '������',0
.83:db '������᪨�, ������᪨� ��',0
.84:db '��ய�',0
.85:db '䥢ࠫ�',0
.86:db '�࠭��᪨�, �����᪨� ��',0
.87:db '��⭨�',0
.88:db '����檨�, ����檨� ��',0
.89:db '����',0
.90:db '���ࠫ��',0
.91:db '�룠�᪨�, �룠���',0
.92:db '��������⠭��',0
.93:db '������� ����⢥���� �����',0
.94:db '����',0
.95:db '�������',0
.96:db '�������',0
.97:db '�����᪨�, �����',0
.98:db '�',0
.99:db '� ��ᯮ����� � �����',0
.100:db '��� �����⭮, � ����',0
.101:db '�������',0
.102:db '� ����',0
.103:db '� ���������� � ������',0
.104:db '� ᮢᥬ ����',0
.105:db '� ����',0
.106:db '��ࠨ��',0
.107:db '�����-�࠭����',0
.108:db 'ﭢ���',0
.109:db '������',0
.110:db '���',0
.111:db '���',0
.112:db '����',0
.113:db '������',0
.114:db '����',0
.115:db '����㪪�',0
.116:db '���쪮�',0
.117:db '���ᮭ',0
.118:db '����쭨檨�',0
.119:db '��஢��ࠤ',0
.120:db '����',0
.121:db '�����',0
.122:db '�㣠��',0
.123:db '����',0
.124:db '�좮�',0
.125:db '����',0
.126:db '���',0
.127:db '�������',0
.128:db '�������쭨�',0
.129:db '��������',0
.130:db '���������� ����',0
.131:db '���',0
.132:db '�����',0
.133:db '������',0
.134:db '�����',0
.135:db '�����',0
.136:db '����宬�',0
.137:db '�������᪨�',0
.138:db '�ॣ��',0
.139:db '�᫮',0
.140:db '��⠢�',0
.141:db '�����⠭',0
.142:db '������',0
.143:db '��ਦ',0
.144:db '��த��� ���㡫��� �������',0
.145:db '���⠢�',0
.146:db '������',0
.147:db '�����',0
.148:db 'ਬ᪨�, ਬ�ﭨ�',0
.149:db '���',0
.150:db '�����',0
.151:db '���᪨�, ���᪨� ��',0
.152:db '�㡡��',0
.153:db '��⫠����',0
.154:db '�⫠��᪨�',0
.155:db 'ᥭ����',0
.156:db '����ய���',0
.157:db '�㬬�',0
.158:db '����ᥭ�',0
.159:db '⥫������',0
.160:db '����-����',0
.161:db '��୮����',0
.162:db '�����',0
.163:db '�⢥�',0
.164:db '��୨�',0
.165:db '���檨�',0
.166:db '��ப',0
.167:db '�����',0
.168:db '��ࠨ��',0
.169:db '�ࠨ�᪨�, �ࠨ���, �ࠨ�᪨� ��',0
.170:db '����-����',0
.171:db '��� �����᪨� ��樠�����᪨� ���㡫��',0
.172:db '����த',0
.173:db '��ମ��',0
.174:db '����',0
.175:db '��⭠�',0
.176:db '������',0
.177:db '��ࣨ���',0
.178:db '������ࠤ',0
.179:db '�।�',0
.180:db '�������⮭',0
.181:db '�������� ��ࣨ���',0
.182:db '७⣥���᪨� ���',0
.183:db '����஦�',0
.184:db '��⮬��',0
.185:db '�������, �����',0
.186:db '��������',0
.187:db '᮪�饭��',0
.188:db 'ᯮᮡ�����, 㬥���',0
.189:db 'ᯮᮡ��',0
.190:db '����ଠ���',0
.191:db '�� �����, �� ��ࠡ��',0
.192:db '�⬥����, �ࠧ�����',0
.193:db '�⬥��, �ࠧ������',0
.194:db '����⥫��, ��⨢��',0
.195:db '�����������',0
.196:db '�����������',0
.197:db '��㣮�, �����, �ਡ����⥫쭮',0
.198:db '���, ���, ���',0
.199:db '᮪����',0
.200:db '�� �࠭�楩',0
.201:db '१���, ��������, ���⮩,���뢨���',0
.202:db '����',0
.203:db '�����⢨�',0
.204:db '��������騩',0
.205:db '�����',0
.206:db '��᮫���, ����᫮���, ����࠭�祭�',0
.207:db 'ᮢᥬ, ᮢ��襭��',0
.208:db '��������, ���뢠��, ����뢠��',0
.209:db '�����ন������',0
.210:db '�⢫�祭��, ����ࠪ��',0
.211:db '������',0
.212:db '���������',0
.213:db '������',0
.214:db '���㯮�ॡ����, �㣠��',0
.215:db '�᪮����',0
.216:db '㤠७��',0
.217:db '�ਭ�����',0
.218:db '�����',0
.219:db '����㯭�',0
.220:db '(�������) ��砩',0
.221:db '��砩��, ���ﭭ�',0
.222:db '����饭��, ����, 䨧���. ����������',0
.223:db 'ᮯ஢������, ��. ���������஢���',0
.224:db 'ᮮ�魨�',0
.225:db 'ᮣ��ᨥ, ᮮ⢥��⢨�, ᮣ��ᮢ���(��), ᮮ⢥��⢮����',0
.226:db 'ᮣ��᭮',0
.227:db 'ᮣ��᭮',0
.228:db 'ᮣ��᭮',0
.229:db 'ᮮ⢥��⢥���',0
.230:db '���, ����',0
.231:db '������ ����, ��������',0
.232:db '��壠���',0
.233:db '�����������(��)',0
.234:db '���',0
.235:db '���������',0
.236:db '�ࠬ. �����⥫�� �����',0
.237:db '��������',0
.238:db '������',0
.239:db '����, ������',0
.240:db '���⨣���',0
.241:db '���⨦����',0
.242:db '����, ��᫮�',0
.243:db '�ਧ������, ���⢥ত��� (����祭��)',0
.244:db '�ਧ�����, ���⢥ত���� (����祭��)',0
.245:db '�����',0
.246:db '�������',0
.247:db '�ਮ�����',0
.248:db '��ࠢ�뢠��',0
.249:db '���',0
.250:db '�१, ᪢���, ����४',0
.251:db '����⢨�, ����㯮�, ⥠��. ���, ����� (�ਭ��� ��ૠ���⮬), ����⢮����, ���� ᥡ�, ����� (஫�)',0
.252:db '�६���� �ᯮ����騩 ��易�����, ���',0
.253:db '����⢨�, ����㯮�, ��. ��, ������ ����⢨�',0
.254:db '���⥫��, ��⨢��',0
.255:db '�ࠬ. ����⢨⥫�� �����',0
.256:db '���⥫쭮���, ��⨢�����',0
.257:db '����',0
.258:db '�����',0
.259:db '����⢨⥫��',0
.260:db '�����',0
.261:db '��஬��� 㤮���⢮७��',0
.262:db '��ᯮᮡ�����, ��।����, ��ࠡ�⪠ (������୮�� �ந��������)',0
.263:db '�ਡ������, ���. ᪫��뢠��',0
.264:db '����������, ���. ᫮�����',0
.265:db '��������',0
.266:db '���ᮢ���, ���ࠢ����, ��������, ����, ���饭��, ���',0
.267:db 'ᮮ⢥�����騩, �������',0
.268:db '�ਢ�থ���, ��஭���',0
.269:db 'ᬥ���',0
.270:db '�ࠬ. ��� �ਫ���⥫쭮�',0
.271:db '����稢���, �⪫��뢠��, ������ ����� (� ��ᥤ�����)',0
.272:db '��ࠢ���� (����� � �. �.), 㫠������, ��ᯮᮡ����, �ਫ�������',0
.273:db '�����������, �ࠢ�⥫��⢮',0
.274:db '����ࠫ',0
.275:db '���᪮� ��������⢮',0
.276:db '����饭��',0
.277:db '���������',0
.278:db '�室, �ਧ�����',0
.279:db '���᪠��, ��७. ����᪠��, �ਧ��������',0
.280:db '�室, �ਧ�����',0
.281:db '�����⨬ �� �� ⠪',0
.282:db '�᪨�, �, �����⮪',0
.283:db '��뭮�����, �ਭ�����',0
.284:db '��뭮������, �ਭ�⨥',0
.285:db '�������',0
.286:db '�����',0
.287:db '�����',0
.288:db '�த������(��), �������(��), �뤢�����, �த�������, �����',0
.289:db '�२����⢮',0
.290:db '�ਪ��祭��',0
.291:db '�᪠⥫� �ਪ��祭��, ���������',0
.292:db '�⢠���, �।�ਨ�稢�',0
.293:db '�����⢨�, ������ �ਪ��祭��',0
.294:db '�ࠬ. ���稥',0
.295:db '��⨢���',0
.296:db '����⢨�, �������',0
.297:db '�������, ४����',0
.298:db 'ᮢ�� (���⠢�����)',0
.299:db 'ᮢ�⮢���, �������, 㢥�������',0
.300:db '����㯠�� ��, ������, ��஭���, ���⭨�',0
.301:db '�������, ��⥭��',0
.302:db '��த஬',0
.303:db '��ய���, ᠬ����',0
.304:db '����',0
.305:db '(���)����⢮����, ���ࠣ�����, ��ࠦ��� (� ���஢�)',0
.306:db '��⢮��⢮',0
.307:db '�ਢ易������',0
.308:db '���騩, �����',0
.309:db '�⢥ত���',0
.310:db '�⢥न⥫��',0
.311:db '���⠢����, ��������� ᥡ�',0
.312:db '�᪮࡫����',0
.313:db '�������',0
.314:db '������',0
.315:db '��᫥, ��',0
.316:db '� ���� ���殢',0
.317:db '�६� ��᫥ ���㤭�',0
.318:db '��⮬, ���᫥��⢨�',0
.319:db '�����, ᭮��',0
.320:db '��⨢',0
.321:db '������, ���',0
.322:db '����५�',0
.323:db '�����⢮, �।�⢮',0
.324:db '�����⪠ ���',0
.325:db '�����, �।�⠢�⥫�',0
.326:db '���㡫���, �����, ࠧ�. ࠧ�ࠦ���, ���������',0
.327:db '������',0
.328:db '���������, ����㦤���, ����஢���',0
.329:db '������',0
.330:db '⮬� �����',0
.331:db '��ࠤ����, ������',0
.332:db 'ᮣ�������, ������ਢ�����, 㦨������, ᮮ⢥��⢮����',0
.333:db '�����',0
.334:db 'ᮣ��ᨥ, ᮣ��襭��, �������',0
.335:db 'ᥫ�᪮宧��⢥���',0
.336:db 'ᥫ�᪮� 宧��⢮',0
.337:db '���।, ���।�',0
.338:db '������, ��������',0
.339:db '楫�, 楫�����, ��६�����',0
.340:db '������, �������, ����樮���, �஢��ਢ���, ���',0
.341:db '��������',0
.342:db 'ᠬ����, ������',0
.343:db '������',0
.344:db '�ॢ���, (��)�ॢ�����',0
.345:db '�㤨�쭨�',0
.346:db '��',0
.347:db '��������',0
.348:db '���⥫��, �஢���',0
.349:db '������',0
.350:db '�����࠭��, �㦤�, �����࠭��',0
.351:db '�室��� (� �ࠬ��� � �. �.), ��᪠���� (� ᠬ����), ���� (� ���)',0
.352:db '���������',0
.353:db '�����',0
.354:db '���騩',0
.355:db '����, ���, ���, ��',0
.356:db '�� ��� ���������',0
.357:db '�� �ᥬ ���',0
.358:db '�����, ���',0
.359:db '�⢥ত���, ��뫠����, �ਢ����� (� ���⢥ত����)',0
.360:db '�।�������, ��୮���',0
.361:db '�����, ���㫮�',0
.362:db '��, த�⢮',0
.363:db '��, த�⢥���',0
.364:db '��������',0
.365:db '���⮪',0
.366:db '���������, ����᪠��',0
.367:db '��ᮡ��, ॣ��ୠ� �������� ������',0
.368:db '�ਬ���, ᯫ��',0
.369:db '㯮������',0
.370:db '��뫠����',0
.371:db '�����',0
.372:db '���, ᮥ������',0
.373:db '�������, ���������',0
.374:db '����',0
.375:db '����',0
.376:db '�����, ��',0
.377:db '�����',0
.378:db '�஬��, ����',0
.379:db '��䠢��, ���㪠',0
.380:db '��쯨�᪨�',0
.381:db '㦥',0
.382:db '⠪��, ⮦�',0
.383:db '(����)��������(��)',0
.384:db '���������, ��६���',0
.385:db '��६����, �।�����(��)',0
.386:db '�롮�',0
.387:db '���',0
.388:db '����',0
.389:db 'ᮢᥬ',0
.390:db '�������',0
.391:db '���ᥣ��',0
.392:db '��ꥤ������, ᮥ�������',0
.393:db '�⥫�, �⥫�᪨�',0
.394:db '㤨�����, ��㬫���',0
.395:db '㤨������, ��㬫����',0
.396:db '��᮫',0
.397:db '����, ����',0
.398:db '�����᫥���',0
.399:db '��⮫�, ��६�����',0
.400:db '��⮫��',0
.401:db '������� ��ᯨ⠫�, ���� ᪮ன �����',0
.402:db '��ᠤ�',0
.403:db '�����饭��',0
.404:db '����, ����',0
.405:db '��㦥᪨�, ��㦥�⢥���',0
.406:db '�।�',0
.407:db '�।�',0
.408:db '������',0
.409:db '����ਯ���',0
.410:db '�������',0
.411:db '�����',0
.412:db '�।�',0
.413:db '�㬬�, �⮣, ������⢮, ��室��� ��, ࠢ������',0
.414:db '������',0
.415:db '�����஢���',0
.416:db '���������, ࠧ�������',0
.417:db '������, ࠧ���祭��',0
.418:db '�������, ������',0
.419:db '����� ���',0
.420:db '��મ�, �����������饥',0
.421:db '�������஢���, ࠧ�����',0
.422:db '������',0
.423:db '������',0
.424:db '���⮬��',0
.425:db '�।��',0
.426:db '类��',0
.427:db '�ॢ���',0
.428:db '�',0
.429:db '�����',0
.430:db '����',0
.431:db '���. 㣮�',0
.432:db '�न��',0
.433:db '��ࠤ����',0
.434:db '����⭮�, ������',0
.435:db '���������',0
.436:db '�ࠦ��������',0
.437:db '騪���⪠',0
.438:db '��ᮥ������, ������஢���, �ਫ������, ����ன��',0
.439:db '��ᮥ�������, �������',0
.440:db '㭨�⮦���',0
.441:db '㭨�⮦����',0
.442:db '�����騭�, ���',0
.443:db '������',0
.444:db '�������',0
.445:db '�����',0
.446:db '��ᠦ����, ���������',0
.447:db '�������, ��������, ���������',0
.448:db '��㫨஢���, �⬥����, 㭨�⮦���',0
.449:db '��㣮�',0
.450:db '�⢥�, �⢥���',0
.451:db '��ࠢ��',0
.452:db '������᪨�',0
.453:db '����',0
.454:db '��⨢��������, ������',0
.455:db '�������᪨�',0
.456:db '�������, �।������',0
.457:db '��������, �।���襭��',0
.458:db '��⨢�廊�',0
.459:db '��⨯���',0
.460:db '�����, �ॢ���',0
.461:db '�ॢ�����',0
.462:db '���������',0
.463:db '��ᯮ����⢮, �����, �ॢ���',0
.464:db '�����祭��, ���ॢ�����',0
.465:db '�����-�����, ��',0
.466:db '���� ����砭��',0
.467:db '��-�����',0
.468:db '�� ��类� ��砥',0
.469:db '��, ��直�, ��-�����',0
.470:db '��-�����, �� 㣮���',0
.471:db '�� ��类� ��砥',0
.472:db '���-�����, �㤠-�����, ��� 㣮���, ����� 㣮���',0
.473:db '� ��஭�, �⤥�쭮, �஧�',0
.474:db '�� ����. �஬�',0
.475:db '������, ������',0
.476:db '�����ﭠ (祫�������ࠧ���)',0
.477:db '�� ����',0
.478:db '����������',0
.479:db '���������',0
.480:db '㦠���, �㣠��',0
.481:db '������, �ਡ��',0
.482:db '�祢����, ��-��������',0
.483:db '�ਧ�, ���饭��, ���������., ��. ��������, ��������, ��뢠��, ��. ������஢���',0
.484:db '�������, ��������, ��⢮����',0
.485:db '������, ���㦭����, ���',0
.486:db '㬨�⢮����, �ᯮ�������',0
.487:db '�ਫ������, ����. ���������',0
.488:db '������, ���, �������',0
.489:db '������஢���, 墠����',0
.490:db '������ᬥ���, ��墠��',0
.491:db '���',0
.492:db '���',0
.493:db '�ਡ��, ��ᯮᮡ�����',0
.494:db '���⥫�, ��⥭���� (�� ����, ���������)',0
.495:db '���졠, ������, �ਬ������, �ਫ������',0
.496:db '�ਪ������',0
.497:db '�ਫ�����, �ਬ�����, 㯮�ॡ���� (� ����)',0
.498:db '�������� ��',0
.499:db '�������� �',0
.500:db '��������',0
.501:db '�����祭��, ���������, ᢨ�����',0
.502:db '�業�����',0
.503:db '������, ���⨬�',0
.504:db '業���, �⤠���� �������, �業�����',0
.505:db '�祭��, ���������',0
.506:db '�祭���⢮',0
.507:db '�ਡ��������, ���室���, �ਡ�������, ���室',0
.508:db '����७��',0
.509:db '���室�騩, ᮮ⢥�����騩, ��ᢠ�����',0
.510:db '����७��',0
.511:db '��������',0
.512:db '�ਡ����⥫��',0
.513:db '��ਪ��',0
.514:db '����',0
.515:db '���室�騩, 㬥���, ᪫����',0
.516:db '�����',0
.517:db '�ந������',0
.518:db '��ᥤ��',0
.519:db '���. �㣠',0
.520:db '�ઠ',0
.521:db '���५�, ��堨�᪨�',0
.522:db '��娥��᪮�',0
.523:db '���⥪��',0
.524:db '���⥪���',0
.525:db '������, ����᪨�',0
.526:db '�뫪��',0
.527:db '���, ��, ࢥ���',0
.528:db '�殮��, ����殮���',0
.529:db '��������',0
.530:db '��� �� ����⮢ � �த���',0
.531:db '����࠭�⢮, ���頤�, ࠩ��, �������, ����',0
.532:db '�७�',0
.533:db 'ᯮ���, �����뢠��',0
.534:db '�����, ᯮ�',0
.535:db '��宩, ����諨��',0
.536:db '���������, �������',0
.537:db '��䬥⨪�',0
.538:db '�㪠 (�� ���� �� ����), ��㦨�, ����㦠��(��)',0
.539:db '��᫮',0
.540:db '����㦥���',0
.541:db '����㦥�� ᨫ�',0
.542:db '��६�ਥ',0
.543:db '�஭�',0
.544:db '�஭�஢����, �஭�����',0
.545:db '��ᥭ��, ����. ��㦥��� �����',0
.546:db '��㦨�',0
.547:db '�ନ�',0
.548:db '������, �����',0
.549:db '�����, ��㣮�',0
.550:db '�㤨��, �஡㦤���',0
.551:db '���⠢����, �ᯮ������ �� ����� (������, 梥�� � �. �.), ���ࠨ����',0
.552:db '���ன�⢮, �ᯮ�������, �ਣ�⮢�����',0
.553:db '���⮢뢠��, �ਪ��뢠�� (��������), ����',0
.554:db '�ਡ�⨥',0
.555:db '�ਡ뢠��',0
.556:db '��᮪���ਥ, �����������',0
.557:db '��५�',0
.558:db '�����',0
.559:db '�����⢮, ६�᫮',0
.560:db '���⨭��� ������',0
.561:db '�����',0
.562:db '�����⢥���, ��⢮��',0
.563:db '��⥧',0
.564:db '��⠢�� ���',0
.565:db '��⨫����',0
.566:db '�㤮����',0
.567:db '�㤮���⢥���',0
.568:db '⠪ ���, � � �६� ���, �����',0
.569:db '�� ᪮�쪮',0
.570:db '��� 㪠���� ����, ⠪, ��� ���',0
.571:db '�� ��ᠥ��� ����',0
.572:db '��� ���',0
.573:db '��� ⮫쪮',0
.574:db '�� ��ᠥ���, ���, ⠪',0
.575:db '⠪��',0
.576:db '⠪�� ���',0
.577:db '���宦�����, ���ꥬ',0
.578:db '��⠭�����, 㤮�⮢������',0
.579:db '����, �����, ���, �ᥭ�',0
.580:db '��뤨����',0
.581:db '� ��ॣ�, �� ��ॣ(�)',0
.582:db '� ��஭�, � ��஭�, �⤥�쭮',0
.583:db '��訢���, �����',0
.584:db '�ࠢ������ �',0
.585:db '�������� �����',0
.586:db '�ᨭ�',0
.587:db '���, �ᯥ��, ��஭�',0
.588:db '��६�����',0
.589:db '��६�����',0
.590:db '�ᥫ',0
.591:db '㡨��⢮',0
.592:db '㡨��',0
.593:db '���������, ����, ������, ���ମ����',0
.594:db 'ᮡ����(��), ��. ����஢���',0
.595:db 'ᮡ࠭��, ��ᠬ����',0
.596:db 'ᮣ��ᨥ, ᮣ�������',0
.597:db '�⢥ত���',0
.598:db '���⠨���� ᢮� �ࠢ�',0
.599:db '�⢥ত��� �㢥ਭ���',0
.600:db '��������, ��ᨣ������',0
.601:db '�����祭��, ��।�� (�����⢠, �ࠢ)',0
.602:db '�ᢠ�����(��), ��ᨬ���஢���(��)',0
.603:db '��������, ᮤ���⢮����',0
.604:db '������, ᮤ���⢨�',0
.605:db '���� � �����������',0
.606:db '����魨�, ����⥭�',0
.607:db 'ᮥ������(��), �������, �������, ���⭨�',0
.608:db '����⢮, ���樠��',0
.609:db '�롮�, ����⨬���',0
.610:db '�ਭ����� �� ᥡ�, �।��������, ����᪠��',0
.611:db '㢥����, �������� (����-�.)',0
.612:db '㤨�����, ��㬫���',0
.613:db '㤨������, ��㬫����',0
.614:db '���஭��⨪�',0
.615:db '���஭����',0
.616:db '����, 㡥���, ��娠���᪠� ��祡���',0
.617:db '�, ��, ��, �, �����, ��',0
.618:db 'ᮢᥬ, �����',0
.619:db '����',0
.620:db '�������',0
.621:db '�� �����',0
.622:db '�����',0
.623:db '�⥨��',0
.624:db 'ᯮ��ᬥ�, �⫥�',0
.625:db '�⫥⨪�',0
.626:db '�⫠�',0
.627:db '�⬮���',0
.628:db '�⮬',0
.629:db '�⮬��� �����',0
.630:db '�⮬��',0
.631:db '����᪨�, ���⮪��, ࠧ�. ����⥫��',0
.632:db '�����⢮',0
.633:db '�ਪ९����, ��ᮥ������, ��७. �ਢ�뢠��, �ਤ����� (���祭��)',0
.634:db '�ਢ易������, �ਪ९�����',0
.635:db '�⠪�����, ��������, �⠪�, ���������, �ਯ����, ����� (�������)',0
.636:db '���⨣����, ��������',0
.637:db '����⪠, ����襭��, �������, ���������',0
.638:db '㤥���� ��������, ����� (�����⥫쭮), ���������',0
.639:db '���頥�����',0
.640:db '�㦨⥫�, �㣠',0
.641:db '��������',0
.642:db '�����⥫��',0
.643:db '�ठ�',0
.644:db '�⭮襭��, ����',0
.645:db '����७��, �������',0
.646:db '�������, �ਢ������',0
.647:db '���殮���, �⥭��, �ਢ����⥫쭮���',0
.648:db '�ਢ����⥫��, �����稢�',0
.649:db '᢮��⢮, �ਧ���, �ࠬ. ��।������, �ਯ��뢠��, �⭮��� (�)',0
.650:db '��樮�',0
.651:db '�⢠���, ��৪��',0
.652:db '�⢠��, ��堫��⢮',0
.653:db '����, ��訬�',0
.654:db '�㤨����, �㤨���, �㡫���, ���⥫�',0
.655:db '�⪠, ���',0
.656:db '��஢�, ��ண��',0
.657:db '��������',0
.658:db '����',0
.659:db '�������',0
.660:db '������, �������稥, �����, ������',0
.661:db '㯮�����稢���',0
.662:db '��⮡������᪨�',0
.663:db '��⮡������᪨�',0
.664:db '��⮡������',0
.665:db '��⮬���᪨�',0
.666:db '��⮬�����',0
.667:db '��⮭����',0
.668:db '�ᥭ�',0
.669:db '�ᯮ����⥫��, �������⥫��',0
.670:db '������, ����騩�� � �ᯮ�殮���',0
.671:db '᭥��� �����, ������',0
.672:db '��筮���, �㯮���',0
.673:db '(��)�����',0
.674:db '��ᯥ��, �����',0
.675:db '�।���',0
.676:db '���饭��',0
.677:db '�⢥������, ������, �⢮����, �।������',0
.678:db '������',0
.679:db '��������, 㪫�������',0
.680:db '��������',0
.681:db '���㦤���, ���ࠦ����',0
.682:db '����, � ������⢨�',0
.683:db '(������������) ����',0
.684:db '㦠��',0
.685:db '㦠᭮, ࠧ�. �祭�, �ࠩ��',0
.686:db '��㪫�, ��������, ��㤮���, ����㤭�⥫��',0
.687:db '⮯��',0
.688:db '���',0
.689:db '����᭠� ������, ���㡮�, ������',0
.690:db 'ॡ񭮪',0
.691:db 'ॡ��᪨�',0
.692:db '�����',0
.693:db '�������筨�',0
.694:db '䮭',0
.695:db '� ���⭮� ���浪�',0
.696:db '�����',0
.697:db '���宩',0
.698:db '�࠭�',0
.699:db '����� ९����',0
.700:db '����',0
.701:db '��讪, �㬪�, ����䥫�',0
.702:db '�����',0
.703:db '���� �� ���㪨',0
.704:db '�ਬ����',0
.705:db '����',0
.706:db '������',0
.707:db '�㫮筠� ������',0
.708:db '������',0
.709:db '���',0
.710:db '������',0
.711:db '������� ��',0
.712:db '����',0
.713:db '�笥��',0
.714:db '�����',0
.715:db '��ࠪ, ������',0
.716:db '���ࠦ�����',0
.717:db '��窠',0
.718:db '��ᯫ����, �������த��',0
.719:db '�����, �ॣࠤ�',0
.720:db '�������',0
.721:db '�᭮�����, ����, �᭮�뢠��, �����, ������',0
.722:db '���ᡮ�',0
.723:db '������',0
.724:db '�᭮����',0
.725:db '⠧, �誠, ��᪠, �����. ���ᥩ�',0
.726:db '�᭮�����, �����, ����',0
.727:db '��২��',0
.728:db '��᪥⡮�',0
.729:db '��᪥⡮����',0
.730:db '�㯠����',0
.731:db '������ ������',0
.732:db '��⢠, ���',0
.733:db '������� ��ࠡ��',0
.734:db '����, �����',0
.735:db '�⨪',0
.736:db '����',0
.737:db '����, ���� � ���ﭨ�',0
.738:db '�����⢮����',0
.739:db '�ਢ몠��',0
.740:db '�������',0
.741:db '�������',0
.742:db '��������',0
.743:db '�न����',0
.744:db '��뤨����',0
.745:db 'ᯠ��',0
.746:db '���� ��⨢',0
.747:db '�����⢮����, �� ᯠ��',0
.748:db '�����',0
.749:db 'த�����',0
.750:db '���� 㢥७��',0
.751:db '���',0
.752:db '���� ����',0
.753:db '����������',0
.754:db '�����뢠��',0
.755:db '�������',0
.756:db '���ॣ�����',0
.757:db '�� ���� ����, ���',0
.758:db '��������',0
.759:db '��न����',0
.760:db '᪮॥',0
.761:db '�᪢������',0
.762:db '���� ��⮢�',0
.763:db '���� �� �ᯮ�������',0
.764:db '���� 㢥७��',0
.765:db '���⢮���� ᥡ� ���',0
.766:db '���',0
.767:db '����',0
.768:db '��ᨢ�',0
.769:db '����, ᤥ����',0
.770:db '��⮬� ��, ⠪ ���',0
.771:db '��-��, �᫥��⢨�',0
.772:db '������, ������',0
.773:db '�⠭�������, ��������',0
.774:db '����ᨯ��',0
.775:db '�஢���',0
.776:db '�祫�',0
.777:db '��',0
.778:db '���廊��',0
.779:db '㫥�',0
.780:db '����',0
.781:db '��ୠ� ᢥ���',0
.782:db '��',0
.783:db '��᭠� ᢥ���',0
.784:db '��, ��।',0
.785:db '�����',0
.786:db '��砫��',0
.787:db '��騩',0
.788:db '��稭���(��)',0
.789:db '᧠��, ��᫥',0
.790:db '���',0
.791:db '�����, �㬠��',0
.792:db '�������, ������',0
.793:db '�����',0
.794:db '�ਭ��������',0
.795:db '�����, ����',0
.796:db '����, ६���',0
.797:db '᪠�����, ����⠪, �⠭��',0
.798:db '��',0
.799:db '�஬�',0
.800:db '������訩',0
.801:db '���訩',0
.802:db '�����',0
.803:db '���ॣ�����',0
.804:db '��㬫����',0
.805:db '����让',0
.806:db '�������',0
.807:db '��������',0
.808:db '���',0
.809:db '���� ஦�����',0
.810:db '���, �୮�',0
.811:db '��⥬�����',0
.812:db '��� �஧�',0
.813:db '����᭠� ��᪠',0
.814:db '�୨��, �୥��',0
.815:db '�㧭��',0
.816:db '������, �������, ����',0
.817:db '��㦤���, ������, ���栭��, �४',0
.818:db '���⮩, ������������, �஡��',0
.819:db '(����ﭮ�) ���﫮',0
.820:db '���� ����, ����, ���뢠��',0
.821:db '�������� ����',0
.822:db '�����',0
.823:db '������',0
.824:db '宫����, ����뭭�, ����',0
.825:db '����',0
.826:db '��梥�',0
.827:db '��㧪�',0
.828:db '����, 㤠�',0
.829:db '���㡮�, ᨭ��',0
.830:db 'ᨭ������',0
.831:db '(��㡠�) �訡��',0
.832:db '�㯮�, १���, ��אַ�',0
.833:db '(��)��᭥��, ��᪠ ��뤠, ��饭��',0
.834:db '��᪠, �⮫, ��⠭��, �ࠢ�����, ��������⢮, ���� (�㤭�)',0
.835:db '������ � �⮫',0
.836:db '���ᨮ�',0
.837:db '���ୠ� (誮��)',0
.838:db '墠����(��)',0
.839:db '�����, �㤭�',0
.840:db '⥫�',0
.841:db '���ᨭ�',0
.842:db '�����',0
.843:db '�����, �����뢠�� �����',0
.844:db '������ 誠�',0
.845:db '����⭠� ����',0
.846:db '�㤥��, �६���, �ந������� ᥭ���, ��, ��, ����让 ���',0
.847:db '��⨭��, ᠯ��',0
.848:db '�㤪�, ����⪠',0
.849:db '���ࠡ����� ����, �����',0
.850:db '�࠭��, �ࠩ, �࠭����, ���������',0
.851:db '��ࠢ���, ��ᢥ૥���� �⢥��⨥',0
.852:db '஦�����',0
.853:db '��⠭���',0
.854:db '���',0
.855:db '���뫪�',0
.856:db '���',0
.857:db '�㯨��',0
.858:db '�騪, ��஡��, ����, ����஢���, 㤠�',0
.859:db '���稭�',0
.860:db '�����',0
.861:db '����',0
.862:db '����稪',0
.863:db '���',0
.864:db '�ମ�',0
.865:db '��⪠, ������, 䨫���',0
.866:db 'ᬥ��, �ࠡ��',0
.867:db 'ᬥ�����, �ࠡ����',0
.868:db '嫥�',0
.869:db '�����, ������',0
.870:db '����ࠪ',0
.871:db '������, ������筠�',0
.872:db '�����',0
.873:db '����',0
.874:db '��७���',0
.875:db '㧤�, �����',0
.876:db '��⪨�',0
.877:db '����䥫�',0
.878:db '�ਣ���, ����',0
.879:db '�ન�, ᢥ��, ��諥���',0
.880:db '������騩, �ਫ�����',0
.881:db '�ࠩ, ���� (���)',0
.882:db '�ਭ����, �ਢ�����',0
.883:db '�����⢫���',0
.884:db '��ᯨ�뢠��',0
.885:db '�ࠩ (���뢠, �ய���)',0
.886:db '�����, �஢���',0
.887:db '�⨭�, (�)�⨭�����',0
.888:db '��㯪��, ������',0
.889:db '�ப��',0
.890:db '᫠����',0
.891:db '����',0
.892:db '��ᨦ����� (樯���), �뢮���',0
.893:db '��祩',0
.894:db '��쮭',0
.895:db '���',0
.896:db '�ਭ����',0
.897:db '���筥��',0
.898:db '�⪠, �����, ����뢠�� (������)',0
.899:db '���⮪��, ����',0
.900:db '�����, ᪮⨭�',0
.901:db '������, ��������, �����',0
.902:db '����',0
.903:db '���',0
.904:db '����, ��ᥪ����, ��',0
.905:db '��ந��',0
.906:db '��ந⥫�',0
.907:db '��஥���',0
.908:db '����ந��',0
.909:db '��, �����',0
.910:db 'ᯠ����, ᣮ���',0
.911:db 'ࠧ��������, ���뢠����',0
.912:db '��⮡��',0
.913:db '����',0
.914:db '�������, �㬠��, ���',0
.915:db '��',0
.916:db '��᫮ (᫨��筮�)',0
.917:db '����窠',0
.918:db '�㣮���, ������, ���⥣�����(��)',0
.919:db '����',0
.920:db '���㯠��',0
.921:db '�㦦���, �㤥��',0
.922:db '(䠡���) �㤮�',0
.923:db '�, �����',0
.924:db '���ﭭ�',0
.925:db '�� �� �� � �� �⠫�',0
.926:db '��⮡�ᮬ',0
.927:db '���⥯����, �ࠤ��, �祭�� �⥯���',0
.928:db '��뢪���',0
.929:db '���।�⢮�',0
.930:db '���� ��, �� � ���� ��砥',0
.931:db '���।�⢮�',0
.932:db '� �������',0
.933:db '������ઠ',0
.934:db '������ ����, �����稪 ⠪�',0
.935:db '(��砭���) ������',0
.936:db '������, ����',0
.937:db '�������',0
.938:db '�����, ������, ����. ⥫��ࠬ��',0
.939:db '���⪠',0
.940:db '���, ��஦����, ��᮪, ���᮪ ���⪠',0
.941:db '��᮪ �뫠',0
.942:db '(��娩���) ����⢨�',0
.943:db '�������, �����뢠��',0
.944:db '��⭠� ��設�',0
.945:db '���������',0
.946:db '⥫����, ��� (����)',0
.947:db '���, �����, (⥫�䮭��) �맮�, �����, �����, ��������, ���뢠��, �㤨��',0
.948:db '�ॡ�����, ��室��� �� ���-�., 祬-�.',0
.949:db '������, �������',0
.950:db '������� (�� ⥫�䮭�), ����. �ਧ뢠��',0
.951:db '��७. �������, �����',0
.952:db 'ᯮ�����, �設�, ������ਥ, �⨫�, �ᯮ�������',0
.953:db '��室��',0
.954:db '��࡫�',0
.955:db '������',0
.956:db '����',0
.957:db '���㫨஢���',0
.958:db '���. ࠪ',0
.959:db '����, ��७���, ��஢����',0
.960:db 'ᢥ�',0
.961:db '���ᢥ筨�',0
.962:db '蠯��, �����',0
.963:db '�������, ᯮᮡ�����',0
.964:db '����训, �������, ���',0
.965:db '�⮫��',0
.966:db '����⠭',0
.967:db '������',0
.968:db '��墠�뢠��, ��墠�',0
.969:db '�����, ��⮬�����',0
.970:db '㣫�த',0
.971:db '����஢��쭠� �㬠��',0
.972:db '���⮭',0
.973:db '�����',0
.974:db '���⭨�',0
.975:db '�����',0
.976:db '����',0
.977:db '�믮�����',0
.978:db '⥫���, �������',0
.979:db '��ਪ����',0
.980:db '�騪, �����, ��砩',0
.981:db '(������) ���죨',0
.982:db '������� ���죨 �� 祪�',0
.983:db '����',0
.984:db '�����',0
.985:db '�����, ���뢠��, ���� (��⠫�), ᫥���',0
.986:db '���',0
.987:db '�����, ��. �����',0
.988:db '���, ��誠',0
.989:db '������',0
.990:db '(ண���) ᪮�',0
.991:db '����⭮����᪠� �ଠ',0
.992:db '�������',0
.993:db '��稭�, �����, ����, ��稭���, ���⠢����',0
.994:db '����',0
.995:db '��⮫��',0
.996:db '�ࠧ�������',0
.997:db '�ࠧ��������',0
.998:db '��६��� �����, �祩��, ����. ����(��)��',0
.999:db '楬��⭮� �����⨥',0
.1000:db '�������',0
.1001:db '業�ࠫ쭨�',0
.1002:db '業��',0
.1003:db '���, �⮫�⨥',0
.1004:db '嫥��� �����, ��୮��',0
.1005:db '����, �६����',0
.1006:db '��।����, �������',0
.1007:db '����筮, ���६����',0
.1008:db '㢥७�����',0
.1009:db 'ᢨ��⥫��⢮, ���䨪��',0
.1010:db '㢥७�����, ��ᮬ��������',0
.1011:db '�४�饭��',0
.1012:db '楯�, 楯�窠, ᪮�뢠��',0
.1013:db '���',0
.1014:db '���',0
.1015:db '祬����',0
.1016:db '祬������',0
.1017:db '蠭�',0
.1018:db '���������, ������ ���죨, ᤠ�',0
.1019:db '����祭��, ��易������',0
.1020:db '(���᪠�) ����, ����ࠬ��',0
.1021:db '�����. ����',0
.1022:db '�������, 㡮���',0
.1023:db '饪�',0
.1024:db '���',0
.1025:db '娬��᪨�',0
.1026:db '��窠 (���᪠�)',0
.1027:db '娬��, ��⥪���',0
.1028:db '娬��',0
.1029:db '����',0
.1030:db '�誨',0
.1031:db '�嬠���',0
.1032:db '���⠭',0
.1033:db '�믫����',0
.1034:db '�����, �㪮����⥫�, ��砫쭨�, ������',0
.1035:db '������ ��ࠧ��',0
.1036:db '����, ॡ񭮪',0
.1037:db '����⢮',0
.1038:db '���',0
.1039:db '�������, ��१���',0
.1040:db '��㡠 (�묮���)',0
.1041:db '�����த��',0
.1042:db '����, ���஢�',0
.1043:db '饯��, ���㦪�, �⪠�뢠��, �⡨����',0
.1044:db '�ਪ���, �ਪ���',0
.1045:db '����᪨�',0
.1046:db '�롮�',0
.1047:db '��',0
.1048:db '�����, ���������',0
.1049:db '�롨���',0
.1050:db '�㡨��, (�⡨����) ��⫥�',0
.1051:db '�����',0
.1052:db '�મ��',0
.1053:db '諠�, ����',0
.1054:db '����(⥠��)',0
.1055:db '���, ���, ���, ��७. ���⮦��⢮, ����஢뢠��',0
.1056:db '���, ���㦭����',0
.1057:db '��',0
.1058:db '��஢���',0
.1059:db '(����让) ��த',0
.1060:db '�ॡ�����, ��⥭������, �⢥ত���, �ॡ������, ��⥭���',0
.1061:db '��, �ਪ�',0
.1062:db '嫮����, ������஢���, 㤠�, 嫮���',0
.1063:db '�⮫��������, �泥, �⠫��������',0
.1064:db '�ਦ����� (� ��㤨), ��������, ᦨ���� (� �㪥), �㪮����⨥, ������, ���⥦��, ��寧�',0
.1065:db '�����',0
.1066:db '�������᭨�',0
.1067:db '����᭠� ������',0
.1068:db '����, 㡨���',0
.1069:db 'ᦨ���� (�㫠��, ���)',0
.1070:db '��客���⢮',0
.1071:db '��饭���',0
.1072:db '稭�����, �㦠騩',0
.1073:db '㬭�, ᯮᮡ��, �����',0
.1074:db '������, ���㯠⥫�',0
.1075:db '����, ᪠��',0
.1076:db '������',0
.1077:db '�������᪨�',0
.1078:db '��थ஡',0
.1079:db '���',0
.1080:db '����뢠��(��), �����, �������, ����',0
.1081:db '�㫠�',0
.1082:db '⪠��, �㪭�, ᪠����',0
.1083:db '�������',0
.1084:db '������, �����, ����',0
.1085:db '���',0
.1086:db '㣮��',0
.1087:db '����०�',0
.1088:db '�����',0
.1089:db '�����',0
.1090:db '�����',0
.1091:db '��᪠',0
.1092:db '������, ���, ���, ��⥬� ᨣ�����, ��஢��� �� ����',0
.1093:db '���',0
.1094:db '��� � ��ୠ�',0
.1095:db '��䥩���',0
.1096:db '�஡',0
.1097:db 'ᢥ��뢠��(��) ����殬, ᯨࠫ��, �����, ᯨࠫ�, ���� (���)',0
.1098:db '�����, 祪����� (������)',0
.1099:db 'ᮢ�������',0
.1100:db '����',0
.1101:db '宫����, 宫����',0
.1102:db '���⭨�, ���⭨箪, �襩���, 宬��',0
.1103:db 'ᮡ����(��)',0
.1104:db 'ᮡ�࠭��, ��������, ᮡ��',0
.1105:db '梥�',0
.1106:db '�������',0
.1107:db '�ॡ���, �ॡ����, ����, ����뢠��',0
.1108:db '�ࠦ����',0
.1109:db '��⠭��',0
.1110:db '��ꥤ�����(��), �����, �.-�. �������, ��������',0
.1111:db '���������',0
.1112:db '��७��, ᣮ࠭��',0
.1113:db '��室���, �ਥ�����',0
.1114:db '���ᥪ���',0
.1115:db '����������',0
.1116:db '��室��� ���६�',0
.1117:db '�������',0
.1118:db '��襭��, 㤮��⢠, �����',0
.1119:db '㤮���, ����',0
.1120:db 'ᬥ譮�, �����᪨�',0
.1121:db '���騩, ����㯠�騩',0
.1122:db '������',0
.1123:db '�ਪ��뢠��, �����������, ��ᯮ��⢮����, �ਪ��, ������������',0
.1124:db '��������, �����砫쭨�, ��������騩',0
.1125:db '墠����',0
.1126:db '�ਬ�砭��, ⮫�������, �������஢���',0
.1127:db '���������',0
.1128:db '�࣮���',0
.1129:db '�࣮��',0
.1130:db '�������, ����祭��',0
.1131:db 'ᮢ����� (����㯫����)',0
.1132:db '������, �������',0
.1133:db '⮢��, �।��� ���ॡ�����',0
.1134:db '��騩, ����⢥���, ��몭������',0
.1135:db '�����쭮���, ��饥 ����, �������',0
.1136:db '��⥭��, ��������',0
.1137:db '����㭠',0
.1138:db 'ᮮ����, ��।�����, ᮮ������',0
.1139:db 'ᮮ�饭��, ���, ����㭨����',0
.1140:db '���㭨��',0
.1141:db '����㭨��, ����㭨���᪨�',0
.1142:db '��騭�, ��魮���',0
.1143:db '��������, �����, ᦠ��, ��७��',0
.1144:db '⮢���, ��⭨�, �����稪',0
.1145:db '����⢮, ��������, ⮢����⢮',0
.1146:db '�ࠢ��⥫��, �⭮�⥫��',0
.1147:db '�ࠢ������',0
.1148:db '�ࠢ�����',0
.1149:db '�⤥�����, �㯥',0
.1150:db '����, ��������, ������, pl ����',0
.1151:db '�������, ���ࠤ����',0
.1152:db 'ᮮ���⢥����',0
.1153:db '���⠢����, ��㦤���',0
.1154:db '��������, ������ࠦ����, �������஢���',0
.1155:db '��������, ������஢���',0
.1156:db '㬥���, �����䨪���, �����⥭⭮���, ����⮪',0
.1157:db '㬥��, ������ࠢ��',0
.1158:db '�ॢ�������, �������, �����७��',0
.1159:db '�����७�, ��᭨� �������',0
.1160:db '��⠢����',0
.1161:db 'ᠬ��������',0
.1162:db '����������',0
.1163:db '������, ���������⢮, �������',0
.1164:db '����������',0
.1165:db '�����, �����稢���, ��������, ���������',0
.1166:db 'ᮢ��襭��',0
.1167:db '᫮���',0
.1168:db '梥� ���',0
.1169:db 'ᮣ��᭮',0
.1170:db '�᫮�����',0
.1171:db '᫮���, ����⠭��',0
.1172:db '����������, �ਢ��, ������ ����������(�)',0
.1173:db '�����, ��஢��',0
.1174:db '��ᯫ��� �������',0
.1175:db '��ᯫ��� �����',0
.1176:db 'ᮣ�������, �ᯮ����� (�����, �������)',0
.1177:db '��⠢����, �稭���, �������',0
.1178:db '�ᯮ���������',0
.1179:db 'ᯮ�����',0
.1180:db '���������',0
.1181:db '���������, ��⠢, ᬥ��, ��몠�쭮� �ந��������, 誮�쭮� �稭����',0
.1182:db '�����騪',0
.1183:db 'ᯮ����⢨�, ᠬ����������',0
.1184:db 'ᬥ��, ��⠢���, �ࠬ. ᫮����稭����',0
.1185:db '����஭���, ����뢠�騩',0
.1186:db '�������',0
.1187:db '�墠�뢠��, �������� (� ᥡ�)',0
.1188:db '����஬���, ���� �� ����஬���, ����஬��஢���',0
.1189:db '�ਭ㤨⥫��, ��易⥫��',0
.1190:db '��������, ���᫨⥫쭠� ��設�',0
.1191:db '⮢���',0
.1192:db '�������',0
.1193:db '��뢠��',0
.1194:db '���㯠��, ����᪠�� (����������� � �. �.)',0
.1195:db '������',0
.1196:db '�᫮���, ���ﭨ�',0
.1197:db '����, ��ਦ�஢���, ���������',0
.1198:db '�������, ��ਦ��, �஢�����',0
.1199:db '�����, �誠 (������ � �. �.)',0
.1200:db 'ᮢ�頭��, ����७��',0
.1201:db '�ਧ��������, �ᯮ����������',0
.1202:db '�ਧ�����, �ᯮ����',0
.1203:db '��������, ����������, �ਧ��������',0
.1204:db '����ਥ, 㢥७�����',0
.1205:db '㢥७��',0
.1206:db '���䨤��樠���, ᥪ���',0
.1207:db '��࠭�稢���, �������� � �����',0
.1208:db '��६��� �����祭��',0
.1209:db '���⢥ত���',0
.1210:db '���⢥ত����',0
.1211:db '���䫨��',0
.1212:db 'ᮮ⢥��⢮����, ���稭�����',0
.1213:db '����ࠢ����',0
.1214:db 'ᮥ������',0
.1215:db '������ ��',0
.1216:db '��᫥����⥫��',0
.1217:db '��襭��',0
.1218:db '�����',0
.1219:db '�९����(��)',0
.1220:db 'ᮣ���� (���)',0
.1221:db '������, �뤠�騩��',0
.1222:db '�������',0
.1223:db '⠩�� ������ਢ�����',0
.1224:db '������⢮',0
.1225:db '����ﭭ�',0
.1226:db '���������',0
.1227:db 'ᮤ�ঠ��, ������',0
.1228:db '���⨭���',0
.1229:db '�த������',0
.1230:db '�����',0
.1231:db '�매�஢�����',0
.1232:db 'ᮧ뢠��',0
.1233:db '㤮��⢮',0
.1234:db '㤮���',0
.1235:db '�ਭ���, �������ᯨ⠭��',0
.1236:db 'ࠧ�����, ��ᥤ�',0
.1237:db '�ॢ�饭��',0
.1238:db '�ॢ����, ������ (� ����� ����)',0
.1239:db '��㪫�',0
.1240:db '��ॢ�����, ��।����� (����, ���)',0
.1241:db '��㦤���, �����祭��, ���ভ��',0
.1242:db '㡥�����',0
.1243:db '㡥��⥫��',0
.1244:db '��嫠���, ᢥ���',0
.1245:db '�ࠢ������ (� �����, ����祩)',0
.1246:db '����',0
.1247:db '�����, �������, ९த���, ᭨���� �����, ����஢���, ���ࠦ��� ����-�.',0
.1248:db '��ࠤ� ��� ��⮯�ᠭ��',0
.1249:db '����᪮� �ࠢ�',0
.1250:db '��ॢ��, ���, ����. �離�',0
.1251:db '�थ��',0
.1252:db '��楢���, ��魮���, ����',0
.1253:db '�஡��, ���몠�� �஡���',0
.1254:db '�⮯��',0
.1255:db '��୮, 嫥��, ����. ����㧠, ����, ������',0
.1256:db '��ᨫ��',0
.1257:db '㣮�',0
.1258:db '����. �����',0
.1259:db '���',0
.1260:db '�ࠢ����, ���४�஢���, ��ࠢ����',0
.1261:db '��ࠢ�����',0
.1262:db '��୮, �ࠢ��쭮',0
.1263:db '��ᬮ�஬',0
.1264:db '��ᬮ����',0
.1265:db '��ᬮ���⨪�',0
.1266:db '��ᬮ�, �ᥫ�����',0
.1267:db '�����',0
.1268:db '業�, �⮨�����, �㤥��� ����প�',0
.1269:db '����',0
.1270:db '����, 娦���, ���⥤�',0
.1271:db '嫮���, (嫮���)�㬠���� ⪠��, (嫮���)�㬠���',0
.1272:db '���',0
.1273:db 'ᮢ��, �������',0
.1274:db '�����',0
.1275:db '�ਫ����, �⮩��',0
.1276:db '��࠭�, ᥫ�᪠� ��᭮���, ��. �����',0
.1277:db '�㦥�⢮',0
.1278:db '�㦥�⢥���',0
.1279:db '����',0
.1280:db '����뢠��, ����뢠��, �������, �墠�뢠��, (��)���誠, ����뢠��, �宫',0
.1281:db '�������, ����������',0
.1282:db '����, �㯮�',0
.1283:db '��஢�',0
.1284:db '����',0
.1285:db '�ࠡ',0
.1286:db '�᪠�뢠��',0
.1287:db '��祭�',0
.1288:db '����᪨����',0
.1289:db '���롥��',0
.1290:db '६�᫮, �㤭�',0
.1291:db '६�᫥����',0
.1292:db '����娢���, ��窠��, ���᪨����',0
.1293:db '��ࠢ��, ���ꥬ�� �࠭',0
.1294:db '�㪮�⪠, ��窠, �㤠�',0
.1295:db '���, �����, ࠧ����(��)',0
.1296:db '�������',0
.1297:db '�����, 㢫�祭��',0
.1298:db '����蠭��',0
.1299:db '�ਯ���',0
.1300:db '᫨���, �६, �६���',0
.1301:db '⢮���, ᮧ������',0
.1302:db 'ᮧ�����, (�)⢮७��',0
.1303:db '⢮��᪨�',0
.1304:db '⢮��, ᮧ��⥫�, ����',0
.1305:db 'ᮧ�����',0
.1306:db '����ਥ, ����, �।��',0
.1307:db '�������, ����',0
.1308:db '�ਪ��',0
.1309:db '�஦��, ��ᥢ, ���, �����ਣ��� (������), 騯��� �ࠢ� (� �������)',0
.1310:db '����, ���ᥪ���, ��॥�����, ���室��� (��ண�, ४�), ��騢��� (��த�)',0
.1311:db '���ન����',0
.1312:db '��४��⮪',0
.1313:db '��९ࠢ�',0
.1314:db '�����, �������',0
.1315:db '��⥭�� (�����)',0
.1316:db '��',0
.1317:db '�㡨�᪨�',0
.1318:db '���誠',0
.1319:db '�������',0
.1320:db '������',0
.1321:db '�誠, �㡮�',0
.1322:db '誠�, ����',0
.1323:db '⢮ண',0
.1324:db '�����, ����ࢨ஢���, �।�⢮, ��祭��',0
.1325:db '������, ������',0
.1326:db '⥪�騩, 室�稩 (� ��ࠦ����� � �. �.), ��⮪, �祭��, �����, �. ⮪',0
.1327:db '�ப������, �ப��⨥',0
.1328:db '������ �⥭��',0
.1329:db '������ �⥭��',0
.1330:db '�����, ������᪠',0
.1331:db '�����',0
.1332:db '१���',0
.1333:db '�㡠��',0
.1334:db '横�',0
.1335:db '����ᨯ�����',0
.1336:db '����',0
.1337:db '����窠',0
.1338:db '������',0
.1339:db '���������',0
.1340:db '������⢮, ��������',0
.1341:db '������ �������',0
.1342:db '����筠� �ଠ',0
.1343:db '��࣠�⪠',0
.1344:db '�����, ���⨭�',0
.1345:db '�।, ���, ������ࠦ����� �� ��⪨',0
.1346:db '������, ���������, �뭨�, ������������',0
.1347:db '⠭��, ���, ⠭楢��',0
.1348:db '⠭殢��, ⠭��',0
.1349:db '��㢠�稪',0
.1350:db '���᭮���, �����',0
.1351:db '�������, �����訢���',0
.1352:db '⥬��, ⥬��',0
.1353:db '⥬�������',0
.1354:db '⥬��������',0
.1355:db '���, ����, ��ண��, ����',0
.1356:db '�⮯���',0
.1357:db '�஬�����, ��',0
.1358:db '�����, 䠪��',0
.1359:db '���',0
.1360:db '��窠',0
.1361:db '����',0
.1362:db '���� �� ����',0
.1363:db '��室��� ����',0
.1364:db '�������',0
.1365:db '�����',0
.1366:db '�����, �����⥫� ᮡ��',0
.1367:db '��ண��, ���',0
.1368:db 'ᬥ���',0
.1369:db '����',0
.1370:db '�������',0
.1371:db '����⨫�⨥',0
.1372:db '�����⥭��, ࠧ��襭��',0
.1373:db '����(��)',0
.1374:db '�������, �����筠� �஡�',0
.1375:db '����஢뢠��',0
.1376:db '�襭��, �訬����',0
.1377:db '���騩, ��⥫��',0
.1378:db '���㡠',0
.1379:db '������, �஢��������, �����',0
.1380:db '�����',0
.1381:db '��襭��',0
.1382:db '����⢨�, ����㯮�',0
.1383:db '�����, ����',0
.1384:db '��������, ��ࠧ���',0
.1385:db '������⮪, ������',0
.1386:db '����ࠢ��, ���������, �������業��, ��䥪⨢��',0
.1387:db '����, ���஭�',0
.1388:db '��������',0
.1389:db '������',0
.1390:db '���஭�⥫��, ���஭�',0
.1391:db '����⥫쭮���',0
.1392:db '��뢠�騩',0
.1393:db '������⮪, �����',0
.1394:db '��।�����, ��⠭��������',0
.1395:db '��।����',0
.1396:db '��।������',0
.1397:db '������ࠦ����',0
.1398:db '��㯥��, �⥯���',0
.1399:db '�����⢮',0
.1400:db '�������',0
.1401:db '�ॡ������, �ॡ�����',0
.1402:db '�����뢠��, ��������஢���, �����뢠��',0
.1403:db '�����, ����������, ������⥫��⢮',0
.1404:db '��૮��, ��⮭',0
.1405:db '���栭��',0
.1406:db '������, �⪠�뢠��(��)',0
.1407:db '��뢠��, 㥧����',0
.1408:db '�⤥�, ������⢮, �����⠬���, ����. ��������⢮',0
.1409:db '㭨��ଠ�',0
.1410:db '����뢠��',0
.1411:db '���祢��',0
.1412:db '������, �������� (� ����), �⫠����, ������ �ᠤ��, �����, ����⮪, �ᠤ��',0
.1413:db '����, ᪫��, ����. ���������஦��� �⠭��',0
.1414:db '�����',0
.1415:db '��㡨��',0
.1416:db '������, �������, ������⥫�',0
.1417:db '����뢠��',0
.1418:db 'ࠧࠡ�⪠, ����, ����',0
.1419:db 'ࠧࠡ��稪, ���������',0
.1420:db '�������',0
.1421:db '���쬥��� �⮫, �����, ����',0
.1422:db '��ᯮ��᪨�',0
.1423:db '(�।)�����祭��, ���� �����祭��',0
.1424:db '��졠',0
.1425:db 'ࠧ�����, 㭨�⮦���',0
.1426:db '����, ����. �⤥�����',0
.1427:db 'ࠧ������(��), ����� (��)',0
.1428:db 'ࠧ��⨥, ������ (��)',0
.1429:db '�⪫������, �����. 㪫��',0
.1430:db '����, �奬�, �஥��, �����, ��ᯮᮡ�����, ��堭���',0
.1431:db '���',0
.1432:db '�ਤ�뢠��, ��������',0
.1433:db '�������',0
.1434:db '�।�������',0
.1435:db '�������',0
.1436:db '��',0
.1437:db '�஢���⢮, ��������',0
.1438:db '����ࠬ��, �奬�',0
.1439:db '��࡫��, ������� ����� (�� ⥫�䮭�)',0
.1440:db '��������᪨�',0
.1441:db '��������᪨� ���ਠ����',0
.1442:db '������',0
.1443:db '�������',0
.1444:db '�����, �ਫ�����',0
.1445:db '���⠭�',0
.1446:db 'ᤥ����',0
.1447:db '㬨���',0
.1448:db '�⫨���, ࠧ�����',0
.1449:db '�⫨��, ࠧ��',0
.1450:db '��㤭�',0
.1451:db '����, ������, �����뢠��',0
.1452:db '��ॢ�ਢ���',0
.1453:db '���⮩��',0
.1454:db '���⮨��⢮',0
.1455:db '�ਫ������',0
.1456:db '�ਫ����',0
.1457:db '�ய',0
.1458:db 'אַ窠 (�� 饪�)',0
.1459:db '�������',0
.1460:db '�����-���࠭',0
.1461:db '�⮫����',0
.1462:db '����',0
.1463:db '��㭠��',0
.1464:db '������',0
.1465:db '��אַ�, ���ࠢ����, �㪮������, �ਪ��뢠��',0
.1466:db '���ࠢ�����, 㪠�����, �㪮����⢮',0
.1467:db '��אַ, ����',0
.1468:db '���᭠� �����, �ࠢ�筨�',0
.1469:db '����, ��吝�',0
.1470:db '��吝�',0
.1471:db '�᪠��祭��, �뢥����� �� ����',0
.1472:db '���뢠��',0
.1473:db '������ਢ���',0
.1474:db '���',0
.1475:db '��ᯫ��, �⮡ࠦ���',0
.1476:db '���⠭��, ����ﭨ�',0
.1477:db '������',0
.1478:db '������',0
.1479:db '㭨�⮦���',0
.1480:db '������ ���㯪�',0
.1481:db '���� �����',0
.1482:db '���, �����',0
.1483:db '���㬥��',0
.1484:db 'ᮡ���',0
.1485:db '㯮��',0
.1486:db '�㪫�',0
.1487:db '������',0
.1488:db '�㯮�',0
.1489:db '�����',0
.1490:db '�����',0
.1491:db '���',0
.1492:db '�����',0
.1493:db '����',0
.1494:db '�६���',0
.1495:db '���',0
.1496:db '����(��)',0
.1497:db '�ࠬ���᪨�',0
.1498:db '�믨��',0
.1499:db '⪠��, �࠯�஢��',0
.1500:db '��⥫�� ����',0
.1501:db 'ᨫ쭮� �।�⢮',0
.1502:db '᪢����, ����᮪, �୮���',0
.1503:db '�����, ����, �ਢ������, �����, �ᮢ���, �௠��',0
.1504:db '�ਡ��������',0
.1505:db '�騪 (�뤢�����)',0
.1506:db '����ᮭ�',0
.1507:db '��㭮�',0
.1508:db '������',0
.1509:db '᭨����',0
.1510:db '����, ���',0
.1511:db '�����, ������, ���������',0
.1512:db '���⭮�',0
.1513:db '����, �ᮢ���',0
.1514:db '��襭�',0
.1515:db '����',0
.1516:db '����',0
.1517:db '����⥫�',0
.1518:db '�ਣ����, �ਥ���',0
.1519:db '�����, ������',0
.1520:db '������, �����騩��, ���᫮������, �맢����',0
.1521:db '���������, �᫥��⢨�',0
.1522:db '������',0
.1523:db '���殣',0
.1524:db '�㯮�, ����, ����, ��᪫�, ������, ���㯫����',0
.1525:db '�㯮� ������',0
.1526:db '�����, ���᫮����, ����. ���⮫����',0
.1527:db '�������, �����',0
.1528:db '���⢠ ������, ���⠪, �����뢠��, ������ �� ���',0
.1529:db '�㡫����, �����',0
.1530:db '����',0
.1531:db '�த����⥫쭮���',0
.1532:db '� �祭��, � �த�������',0
.1533:db '�㬥ન',0
.1534:db '���, �����, ���',0
.1535:db '��쭠� ��ﯪ�',0
.1536:db '����',0
.1537:db '����, ��易������, ��諨��',0
.1538:db '��૨�',0
.1539:db '����',0
.1540:db '�����࠭����� (� 祬-����)',0
.1541:db '����',0
.1542:db '��᪨, �����',0
.1543:db '��襭�',0
.1544:db '㬨��騩, �।ᬥ���',0
.1545:db '��������',0
.1546:db '�����',0
.1547:db '��� ��㣠',0
.1548:db '�뫪��',0
.1549:db '�५',0
.1550:db '��, �����',0
.1551:db '࠭���',0
.1552:db '��ࠡ��뢠��, ���㦨����',0
.1553:db '��쥧��, ����稩, 㡥��⥫��',0
.1554:db '��ࠡ�⮪',0
.1555:db '�����',0
.1556:db '��������ᥭ��',0
.1557:db '��������',0
.1558:db '���졥��',0
.1559:db '�����',0
.1560:db '���⮪',0
.1561:db '������',0
.1562:db '������, ���ਭ㦤���� (� ������)',0
.1563:db '����',0
.1564:db '�������',0
.1565:db '��ࠧ������',0
.1566:db '�ᨫ��',0
.1567:db '��ᥬ�',0
.1568:db '���쬮�',0
.1569:db '���訩 (� ᥬ�)',0
.1570:db '�������',0
.1571:db '�����᪨�',0
.1572:db '�����⠭��',0
.1573:db '�����',0
.1574:db '�����, ����, �᭮�� (��㪨 � �. �.)',0
.1575:db '�������, (��ࢮ-)��砫��',0
.1576:db '᫮�',0
.1577:db '���������, ��������',0
.1578:db '����������',0
.1579:db '����������',0
.1580:db '��',0
.1581:db '��᮫��⢮',0
.1582:db '�ਮ�, ��த��',0
.1583:db '�����',0
.1584:db '�������',0
.1585:db '�����',0
.1586:db '�ࠣ',0
.1587:db '����, �����⥫�',0
.1588:db '�������',0
.1589:db '��᫠�������',0
.1590:db '�������(��); 㢥��稢���(��)',0
.1591:db '��ᢥ���',0
.1592:db '����㯠�� �� ������� �㦡�',0
.1593:db '�ࠦ��',0
.1594:db '�஬����',0
.1595:db '�����筮',0
.1596:db '��������',0
.1597:db '��ࠡ����',0
.1598:db '����⠭��',0
.1599:db '�室���, ���㯠��',0
.1600:db 'ᮡ�������',0
.1601:db '�室, ���㯫����, ������',0
.1602:db '��������',0
.1603:db '�����稢���',0
.1604:db '�������',0
.1605:db '�����⫨��',0
.1606:db '�।�, ���㦥���',0
.1607:db '��',0
.1608:db '����� (१�����), ��७. �����������',0
.1609:db '����������, ᮮ�㦠��, ��אַ�',0
.1610:db '�ᮡ����',0
.1611:db '���',0
.1612:db '��⠭��������',0
.1613:db '��筮���',0
.1614:db '���',0
.1615:db '����',0
.1616:db '����',0
.1617:db 'ᮡ�⨥',0
.1618:db '�����-�����',0
.1619:db '�१ ����',0
.1620:db '�����, ��直�',0
.1621:db '�����, ��直� 祫����',0
.1622:db '�����, ��直� 祫����',0
.1623:db '��',0
.1624:db '��୮�, ���',0
.1625:db '�����, �몠�뢠��',0
.1626:db '��뢠�� (ᨬ���� � �. �.)',0
.1627:db '���',0
.1628:db '�筮',0
.1629:db '��㢥��稢���',0
.1630:db '��������, �ॢ�������',0
.1631:db '�����, �ᬮ��, ��᫥�������',0
.1632:db '����������, �ᬠ�ਢ���, ��᫥������',0
.1633:db '�ਬ��, ��ࠧ��',0
.1634:db '�ॢ��室���',0
.1635:db '�ॢ��室��',0
.1636:db '����㦤���',0
.1637:db '����㦤����, ��������',0
.1638:db '��᪫����',0
.1639:db '��᪫�栭��',0
.1640:db '�᪫����',0
.1641:db '������',0
.1642:db '�ࠦ�����',0
.1643:db '��ࠤ�',0
.1644:db '���⠢��',0
.1645:db '��室',0
.1646:db '�������(��)',0
.1647:db '���७��, �ᯠ���',0
.1648:db '�������, ���������',0
.1649:db '��������',0
.1650:db '�ᯥ����',0
.1651:db '�룮����, �᪫����',0
.1652:db '���, ��室',0
.1653:db '�ᯥਬ���',0
.1654:db '��������',0
.1655:db '��᫥�������',0
.1656:db '��᫥������, ������',0
.1657:db '��᪠�뢠���',0
.1658:db '�������� �� ����⢠',0
.1659:db '��������; ᢥ��',0
.1660:db '����',0
.1661:db '����',0
.1662:db '⪠��',0
.1663:db '���',0
.1664:db '��࡫�� (� �ᮢ)',0
.1665:db '��ਢ�',0
.1666:db '��������, �।�⢠ 㤮��⢠',0
.1667:db '䠪�',0
.1668:db '䠡ਪ�, �����',0
.1669:db '�鸞��',0
.1670:db '�����⠢���, �� 墠���, �������� ��������, �஢������� (�� ������), �� 㤠������',0
.1671:db '��㤠�, �������⢮',0
.1672:db '�४���, ᢥ��, ���, ����',0
.1673:db '����������',0
.1674:db '᪠���',0
.1675:db '������',0
.1676:db '������',0
.1677:db '��������',0
.1678:db 'ᥬ��',0
.1679:db '���������',0
.1680:db '�������, ������',0
.1681:db '�ଠ',0
.1682:db '�ଥ�, �७����',0
.1683:db '��졠',0
.1684:db '���',0
.1685:db '������⮪, �訡��, ����',0
.1686:db '���짠, �����',0
.1687:db '���',0
.1688:db '�ᮡ�������',0
.1689:db '��ନ��',0
.1690:db '䥤�ࠫ��, ��',0
.1691:db '䥤��⨢��',0
.1692:db '������',0
.1693:db '᫠��',0
.1694:db '��ନ��',0
.1695:db '���⢮����, �㯠��, ����뢠��',0
.1696:db '���� ᥡ� ���⢮����',0
.1697:db '���⢮',0
.1698:db '����',0
.1699:db '�㡨��, ������ (��ॢ��), 㯠���',0
.1700:db '�����⢮����',0
.1701:db '��ࠤ�, �����',0
.1702:db '����, �������',0
.1703:db '����',0
.1704:db 'ᢨ९�, ����',0
.1705:db '��⭠����',0
.1706:db '����',0
.1707:db '��⨤�����',0
.1708:db '���줥���',0
.1709:db '�ࠦ�����, �������, ���, �ࠪ�',0
.1710:db '����, ����',0
.1711:db '䨣��, ���',0
.1712:db '䨣�୮� ��⠭��',0
.1713:db '䠩�, �����쭨�, �७��, ��ன, ��, �����, ����, ����⥪�',0
.1714:db '���������(��), ������஢��� (��)',0
.1715:db '䨫�, ������, �ந������� �����ꥬ��',0
.1716:db '��室���',0
.1717:db '�४���',0
.1718:db '�����稢���',0
.1719:db '���',0
.1720:db '�����',0
.1721:db '�����',0
.1722:db '����',0
.1723:db 'ਡ�, �론���',0
.1724:db '�ਯ����, ����, �����',0
.1725:db '����',0
.1726:db '�९����, ��⠭��������, 䨪�஢���',0
.1727:db '����������, ��⠭�������',0
.1728:db '䫠�',0
.1729:db '�寧�',0
.1730:db '���',0
.1731:db '����',0
.1732:db '�����',0
.1733:db '�����',0
.1734:db '���',0
.1735:db '梥⮪',0
.1736:db '�����, �����, ���',0
.1737:db '��ॡ����',0
.1738:db '業�',0
.1739:db '��த��',0
.1740:db '᫥������',0
.1741:db '�����, ���騩',0
.1742:db '���',0
.1743:db '��ࠪ',0
.1744:db '����',0
.1745:db '������筮� �ਬ�砭��, ᭮᪠',0
.1746:db '��⡮�, ��⡮��� ���',0
.1747:db '��⡮����',0
.1748:db '᫥�, ������, ��室��',0
.1749:db '���',0
.1750:db '�����, �����, �����',0
.1751:db '���ᥣ��',0
.1752:db '�� ����ࠪ',0
.1753:db '�����猪',0
.1754:db '���ਬ��',0
.1755:db '�뤠���� �� �ப��',0
.1756:db 'ᨫ�',0
.1757:db '�।��',0
.1758:db '���',0
.1759:db '������',0
.1760:db '���뢠��',0
.1761:db '������',0
.1762:db '�����',0
.1763:db '�ଠ, �ନ஢���, ����� (� 誮��)',0
.1764:db '��㫠, �楯�',0
.1765:db '���।',0
.1766:db '�।���騩, ����騩',0
.1767:db '�ப���',0
.1768:db '�ப',0
.1769:db '�����, �������',0
.1770:db '����吝����, ��吝�, �������',0
.1771:db '��襭����⢮',0
.1772:db '�᭮�뢠��, ����',0
.1773:db '�᭮��⥫�',0
.1774:db '����',0
.1775:db '�⢥���',0
.1776:db '���',0
.1777:db '����',0
.1778:db '��ࠬ����, ࠬ�, ��⮢',0
.1779:db '����. �㤥���� ���業�஢��',0
.1780:db '��ઠ�, ��⮢',0
.1781:db '����⢥��� ��ன',0
.1782:db '��७���, ��஢����',0
.1783:db '��஢���',0
.1784:db '��஢���� ������',0
.1785:db '���᪨�',0
.1786:db '᢮�����, ��ᯫ���, ������',0
.1787:db '��ᯫ�⭮',0
.1788:db '᢮����',0
.1789:db 'ᢥ���',0
.1790:db '���, ���⥫�',0
.1791:db '��㦭�',0
.1792:db '��㦡�',0
.1793:db '��, ��',0
.1794:db '�ᠤ, ��।, �஭�',0
.1795:db '��஧',0
.1796:db '��஧��',0
.1797:db '����஧���, ����৭���',0
.1798:db '�����',0
.1799:db '�窠',0
.1800:db 'ᬥ譮�, �������',0
.1801:db '����, ᫥����',0
.1802:db '��ᥫ�',0
.1803:db '�������, ��ࠡ��뢠��, ���⨣���, �먣�뢠��, �ਡ������ (� ���), �२����⢮',0
.1804:db '��室��',0
.1805:db '�ࠡ��, ��������',0
.1806:db '������',0
.1807:db '��ᥫ��',0
.1808:db '�����',0
.1809:db '����� (� ������ ����)',0
.1810:db '��ப',0
.1811:db '���, ����',0
.1812:db '�ਣ���, �����, ��⥫�, 蠩��, �����',0
.1813:db '������ �������',0
.1814:db '�஫��, �஡��, �஬���⮪',0
.1815:db '������, �������',0
.1816:db '����',0
.1817:db 'ᠤ',0
.1818:db 'ᠤ�����',0
.1819:db '���',0
.1820:db 'ᮡ����',0
.1821:db '����',0
.1822:db '���⠫쭮 ��拉��, ���⠫�� �����',0
.1823:db '�ࠬ. த',0
.1824:db '��騩, ����ࠫ',0
.1825:db '�����, ���筮',0
.1826:db '���������, �������',0
.1827:db '�����',0
.1828:db '��᪮��, �����, ���',0
.1829:db '�������',0
.1830:db '�������, ���⠢���, �⠭�������',0
.1831:db '���ࠢ������',0
.1832:db '�室���',0
.1833:db '��室���',0
.1834:db '�室���, 㦨������, ������ �ᯥ�',0
.1835:db '��⠢���',0
.1836:db 'ᮣ�����',0
.1837:db '�஬������',0
.1838:db '�ਢ������, ���',0
.1839:db '������, �������',0
.1840:db 'ubufyncrbq',0
.1841:db '�������',0
.1842:db '�������, ����祭�',0
.1843:db '����窠, ����誠',0
.1844:db '������',0
.1845:db '�������, ࠤ����',0
.1846:db '������',0
.1847:db '���, ����, ���������',0
.1848:db '���� ��⮡�ᮬ',0
.1849:db '���� �������',0
.1850:db '��� �����',0
.1851:db '��� �����',0
.1852:db '���������� ᯮ�⮬',0
.1853:db '��� ��誮�',0
.1854:db '��室���',0
.1855:db '室��� �� ���������',0
.1856:db '�������� ᯠ��',0
.1857:db '楫�, ᯮ��. ���',0
.1858:db '����',0
.1859:db '���',0
.1860:db '����� �窨, �窨-������',0
.1861:db '�����',0
.1862:db '����⮩, ��������',0
.1863:db '��訩',0
.1864:db '����� ����',0
.1865:db '�� ᢨ�����',0
.1866:db '⮢���',0
.1867:db '����',0
.1868:db '��릮����',0
.1869:db '���⠢���, �������',0
.1870:db '�ࠢ�⥫�, �ࠢ�⥫�',0
.1871:db '�ࠢ�⥫��⢮',0
.1872:db '�㡥ୠ��',0
.1873:db '�����',0
.1874:db '��୮',0
.1875:db '����让',0
.1876:db '�থ�⢥���� ᮡ࠭��',0
.1877:db '���',0
.1878:db '����誠',0
.1879:db '�ࠢ�',0
.1880:db '��⪠',0
.1881:db '������',0
.1882:db '����让',0
.1883:db '������',0
.1884:db '�ਢ���⢮����',0
.1885:db '�ਢ���⢨�',0
.1886:db '�����',0
.1887:db '���',0
.1888:db '�ண����',0
.1889:db 'ᨢ�',0
.1890:db '����⨥, 墠⪠, �墠�뢠��, ��������',0
.1891:db '�����',0
.1892:db '����, ������, ��⮢�',0
.1893:db '����',0
.1894:db '��㯯�',0
.1895:db '���, ���騢���, ࠧ������, �⠭�������',0
.1896:db '�����, ������',0
.1897:db '�����',0
.1898:db '�����',0
.1899:db '���, 㢥��祭��, ���宫�',0
.1900:db '��࠭�஢����',0
.1901:db '�����',0
.1902:db '���, ����ᮢ��',0
.1903:db '砩��',0
.1904:db 'ᬮ��, ����, ᪫������(��)',0
.1905:db '���, �࠭��',0
.1906:db '���� (����)',0
.1907:db '��誨',0
.1908:db '������, �������� �����',0
.1909:db 'ᯮ�⨢�� ���',0
.1910:db '������⨪�',0
.1911:db '��������',0
.1912:db '�ਢ�窠, ���砩',0
.1913:db '�ਢ���, �����',0
.1914:db '�����',0
.1915:db '������',0
.1916:db '�������',0
.1917:db '��������',0
.1918:db '�㪠',0
.1919:db '��ᨢ�',0
.1920:db '�����',0
.1921:db '�������',0
.1922:db '�������',0
.1923:db '�������',0
.1924:db '���⫨��',0
.1925:db '���⫨��',0
.1926:db '��㤭�, ⢥��',0
.1927:db '����',0
.1928:db '�।',0
.1929:db '����।��',0
.1930:db '��ମ����, ��㦭�',0
.1931:db '��ମ���, ᮧ��稥, ᮣ��ᨥ',0
.1932:db '����, ������',0
.1933:db '��஭�',0
.1934:db '�஦��',0
.1935:db '�ﯠ',0
.1936:db '����������',0
.1937:db '�����',0
.1938:db '��� �஢��� �६�',0
.1939:db '������',0
.1940:db '����ࠪ���',0
.1941:db '�������',0
.1942:db '㦨����',0
.1943:db '���� ��易��� ��-� ᤥ����',0
.1944:db '��',0
.1945:db '�� �祭� �⥫ �� �������� ���',0
.1946:db '��� ��� (� ��த� � �. �.)',0
.1947:db '��� ��� ����',0
.1948:db '������',0
.1949:db '��४�� 誮��',0
.1950:db '�⠡',0
.1951:db '���஢�',0
.1952:db '�����',0
.1953:db '�����',0
.1954:db '���',0
.1955:db '�殮��',0
.1956:db '����, ��設�, ���, �����襭�����',0
.1957:db '��᫥����',0
.1958:db '�����, ��ঠ��',0
.1959:db '���⮫��',0
.1960:db '��',0
.1961:db '���',0
.1962:db '諥�, ��᪠',0
.1963:db '��������, ������',0
.1964:db '�����ਥ',0
.1965:db '��',0
.1966:db '���, �',0
.1967:db '���, �������� (�� ���祭�� 祣�-�.)',0
.1968:db '��ன',0
.1969:db '��ந�᪨�',0
.1970:db '��ந��',0
.1971:db 'ᥡ�, ᠬ�',0
.1972:db '��������',0
.1973:db '������(��)',0
.1974:db '�����ࠧ��, �த����, �����',0
.1975:db '��᮪��',0
.1976:db '�।��� 誮��',0
.1977:db '���襥 ��ࠧ������',0
.1978:db '���쬠',0
.1979:db '������ ��ண�, ���',0
.1980:db '���, 宫�',0
.1981:db '���, ���',0
.1982:db 'ᥡ�, ᠬ',0
.1983:db '�����, �९���⢮����',0
.1984:db '�����, �९���⢨�',0
.1985:db '����, �୨�, �����, ������ �� �����, ��७. �������',0
.1986:db '������� ��',0
.1987:db '�����, ��������',0
.1988:db '����',0
.1989:db '��������',0
.1990:db '���, ᢮�',0
.1991:db '�����',0
.1992:db '㤠����',0
.1993:db '审��, ���� ����⨥',0
.1994:db '宪���',0
.1995:db '宪�����',0
.1996:db '��ঠ��',0
.1997:db '��७. ��஭�����',0
.1998:db '��७. ��஭�����',0
.1999:db '�ࠧ����',0
.2000:db '�������',0
.2001:db '���, �����',0
.2002:db '��� �����',0
.2003:db '����譥� �������',0
.2004:db '�।����, ����',0
.2005:db '����, ᫠��, 㢠����',0
.2006:db '���� ������',0
.2007:db '�������, ���������',0
.2008:db '����',0
.2009:db '��ᯨ⠫�, ���쭨�',0
.2010:db '����稩, ��ન�',0
.2011:db '��⥫�',0
.2012:db '��',0
.2013:db '���',0
.2014:db '����宧頻�',0
.2015:db '���',0
.2016:db '��ࠢ����',0
.2017:db '᪮�쪮',0
.2018:db '᪮�쪮',0
.2019:db '᪮�쪮 ⥡� ���',0
.2020:db '��஬��',0
.2021:db '��',0
.2022:db '�� ����',0
.2023:db '���',0
.2024:db '�����',0
.2025:db '࠭���, ������',0
.2026:db '��',0
.2027:db '���',0
.2028:db '��஦���',0
.2029:db '����',0
.2030:db '�������, �����',0
.2031:db '⮦���⢥���',0
.2032:db '�⮦���⢫���(��), ����������',0
.2033:db '���������',0
.2034:db '�����',0
.2035:db '�ࠧ���, ������, ����, ��ᯮ�����',0
.2036:db '����, �㬨�',0
.2037:db '�᫨',0
.2038:db '���쭮�',0
.2039:db '���㦤���',0
.2040:db '���஭�栥��',0
.2041:db '�����㬥��',0
.2042:db '㬮����',0
.2043:db '���ࠧ㬥����, ��������',0
.2044:db '�������, ������, ����',0
.2045:db '��������, ����⥫쭮���',0
.2046:db '�����, ����⥫��',0
.2047:db '����⫥���',0
.2048:db '�, �����',0
.2049:db '� ������ ��⠢�',0
.2050:db '� ᮮ⢥��⢨� �',0
.2051:db '�� �ࠢ����� �',0
.2052:db '� ᮮ⢥��⢨� �',0
.2053:db '� ᮮ⢥��⢨� �',0
.2054:db '����⢨⥫쭮',0
.2055:db '�� ����� (����-�����)',0
.2056:db '��।',0
.2057:db '�����',0
.2058:db '� ��� ���',0
.2059:db '� �⭮襭��',0
.2060:db '�� ᬮ��� ��, ����४��',0
.2061:db '�� ᥫ�',0
.2062:db '� �᭮����',0
.2063:db '�� ���',0
.2064:db '� ���',0
.2065:db '�� ���⨭�',0
.2066:db '����᭮',0
.2067:db '��ᯮᮡ�����',0
.2068:db '������㯭�',0
.2069:db '�� �⢥��騩 �ॡ������, ���������',0
.2070:db '�',0
.2071:db '��������, ᮤ�ঠ��, �������',0
.2072:db '������, � ⮬ �᫥',0
.2073:db '㢥��稢�����, �����⠭��, �����',0
.2074:db '���ਫ���',0
.2075:db '� ᠬ�� ����, ����⢨⥫쭮',0
.2076:db '����।����, ����࠭�祭��',0
.2077:db '������ᨬ����',0
.2078:db '������ᨬ�',0
.2079:db '㪠��⥫�, ������, ������⥫�',0
.2080:db '�஬�諥���',0
.2081:db '�஬�諥������',0
.2082:db '������',0
.2083:db '���ﭨ�',0
.2084:db 'ᮮ����, 㢥�������, ������� (�� ����-����)',0
.2085:db '����樠���',0
.2086:db '���ଠ��, ᮮ�饭��, ᢥ�����',0
.2087:db '������',0
.2088:db '(�)��᫥������',0
.2089:db '��᫥��⢮',0
.2090:db '���祫�����',0
.2091:db '��稭, ���樠⨢�',0
.2092:db '���।���, ࠭���, �᪮ࡨ��',0
.2093:db '�������',0
.2094:db '��⥣ࠫ, 楫�',0
.2095:db '����ᮢ���',0
.2096:db '������',0
.2097:db '����㭠த��',0
.2098:db '�����, ���ࠪ�',0
.2099:db '���ࢥ���',0
.2100:db '�, ��',0
.2101:db '���௨��',0
.2102:db '�������, ����㦤���',0
.2103:db '���������, ४����������',0
.2104:db '������⥫�',0
.2105:db '�ਣ�����',0
.2106:db '������, ���',0
.2107:db '�஭�஢����',0
.2108:db '���஢',0
.2109:db '�����஢���, �⤥����',0
.2110:db '�뤠����, ���᪠��',0
.2111:db '��, ���, ��� (��� �� ����� �।��⮢ � �������)',0
.2112:db '�� ��諮',0
.2113:db '�� ��� ���� �����⥭樨',0
.2114:db '����� ����',0
.2115:db '���� ᭥�',0
.2116:db '�����',0
.2117:db '����� ����',0
.2118:db '�����',0
.2119:db '� �������',0
.2120:db '����� �ணࠬ��, �㭪�, ��ࠣ��, �����',0
.2121:db '���, ��, ᢮�',0
.2122:db '᫮����� �����',0
.2123:db '����',0
.2124:db '������',0
.2125:db '蠪��',0
.2126:db '�����, ���⪠',0
.2127:db '�����',0
.2128:db '祫����',0
.2129:db '����, ��㤥��',0
.2130:db '��ᮥ��������, ���㯠�� (� �����)',0
.2131:db '��⪠',0
.2132:db '��⪨ � ��஭�',0
.2133:db '��ୠ����',0
.2134:db '����, ���⮪, �㤨��',0
.2135:db '�ਣ����, �㦤����',0
.2136:db '��設',0
.2137:db '᮪',0
.2138:db '���',0
.2139:db '��룠��',0
.2140:db '��릪� (��� ᯮ��)',0
.2141:db '⮫쪮 ��, ����',0
.2142:db '��ࠢ�뢠��',0
.2143:db '����',0
.2144:db '��',0
.2145:db '����',0
.2146:db '�����, �஭��뢠�騩, ᨫ��, १���, �஭��⥫�� (�� 㬥, ����拉)',0
.2147:db '�࠭���, ��ঠ��, ᮡ���',0
.2148:db '��� � ����',0
.2149:db '��஦���, ��ࠦ�, ����, ���. ����',0
.2150:db '�࠭�⥫�, ᬮ��⥫�, ��஦',0
.2151:db '(ᮡ����) �����',0
.2152:db '��ঠ��, ��࠭���',0
.2153:db '���稭�',0
.2154:db '㡨����',0
.2155:db '�����ࠬ',0
.2156:db '��������',0
.2157:db '���, ���, �����',0
.2158:db '��஫�',0
.2159:db '��஫���⢮',0
.2160:db '����',0
.2161:db '���த',0
.2162:db '�����',0
.2163:db '�����',0
.2164:db '�����',0
.2165:db '������',0
.2166:db '���, �⨪�⪠',0
.2167:db '��������',0
.2168:db '���',0
.2169:db '��㤮��� ���祭��',0
.2170:db '��㦥��, ��ப',0
.2171:db '�����⢨�, ������⮪, �����⠢���',0
.2172:db '��७�',0
.2173:db '���⭨� (���⠢���)',0
.2174:db '����',0
.2175:db '����� ��஢��',0
.2176:db '����',0
.2177:db '�����',0
.2178:db '�����, ���, ��࠭�',0
.2179:db '宧頻� (����, ���⨭���)',0
.2180:db '宧鶴 (����, ���⨭���)',0
.2181:db '�������������',0
.2182:db '������',0
.2183:db '��ᥫ�筠� ��ண�, ���㫮�',0
.2184:db '�� (���)',0
.2185:db '����让',0
.2186:db '����஭��',0
.2187:db '嫥����, 㤠����, �����',0
.2188:db '����誠',0
.2189:db '��᫥����',0
.2190:db '��� ���஬',0
.2191:db '�������, �����',0
.2192:db '�������',0
.2193:db '⮪��� �⠭��',0
.2194:db 'ᬥ�����',0
.2195:db '�������, ����',0
.2196:db '������',0
.2197:db '����뢠�� �� �⮫',0
.2198:db '����',0
.2199:db '������',0
.2200:db '����, �㪮������, ᢨ���, ��䥫�',0
.2201:db '�����, �㪮����⥫�, �����',0
.2202:db '�㪮����⢮',0
.2203:db '��।���� �����',0
.2204:db '���⮪',0
.2205:db '����, ��',0
.2206:db '��룠��',0
.2207:db '��룠��',0
.2208:db '��룠��',0
.2209:db '������, 㧭�����',0
.2210:db '����',0
.2211:db '����',0
.2212:db '��������',0
.2213:db '����, ������',0
.2214:db '����, ��⠢����',0
.2215:db '����',0
.2216:db '�������',0
.2217:db 'ࠧ���稢�, ��⪨�',0
.2218:db '��������⥫��⢮',0
.2219:db '�⤠������� (����-�.)',0
.2220:db '����訩, �����',0
.2221:db '�ப',0
.2222:db '���������',0
.2223:db '���� ���㬠��',0
.2224:db '���쬮, �㪢�, ����',0
.2225:db '�஢���, ஢��',0
.2226:db 'ᡮ� (�������)',0
.2227:db '�����থ���, ��易���',0
.2228:db '��������',0
.2229:db '���',0
.2230:db '�᢮��������',0
.2231:db '�᢮����⥫�',0
.2232:db '������⥪�',0
.2233:db '������',0
.2234:db '�������',0
.2235:db '�����',0
.2236:db '����',0
.2237:db 'ᢥ�, ᢥ��',0
.2238:db '�������, ��宦��, �ࠢ�����, ���',0
.2239:db '�㡠',0
.2240:db '������� ᯨ��',0
.2241:db 'ᯨ᮪',0
.2242:db '�����',0
.2243:db '�����',0
.2244:db '���⥫�',0
.2245:db '�㪢����',0
.2246:db '��������',0
.2247:db '�ࠬ���',0
.2248:db '�������',0
.2249:db '����',0
.2250:db '��ᨫ��, ����⨫��, ����� (������, 饭��), ��',0
.2251:db '�����쪨�',0
.2252:db '����',0
.2253:db '����� ������',0
.2254:db '�ॢ��, ॣ������',0
.2255:db '������',0
.2256:db '�����',0
.2257:db 'ᬮ����',0
.2258:db '��������� �',0
.2259:db '�᪠��',0
.2260:db '���� ��宦�� (�� ����-�)',0
.2261:db '��㧮��� ��⮬�����',0
.2262:db '�����, �ந��뢠��',0
.2263:db '�����',0
.2264:db '�������',0
.2265:db '��஬���',0
.2266:db '�஬��',0
.2267:db '���',0
.2268:db '������, �娩 (� �����), �����',0
.2269:db '�뭨�',0
.2270:db '���訩, ������, ����, ��������(��)',0
.2271:db '����, �।����',0
.2272:db '��୮���, �।�������',0
.2273:db '�����',0
.2274:db '㡠����, �����, �����',0
.2275:db '��ᮬ��ਠ��',0
.2276:db '����, ��ன ����ࠪ',0
.2277:db '��設�',0
.2278:db '�㫥���',0
.2279:db 'ᤥ����',0
.2280:db '��ୠ�',0
.2281:db '����, ���뫠��, ���⮢�',0
.2282:db '������',0
.2283:db '������',0
.2284:db '���⥫��� �஢���',0
.2285:db '��⠢���� ������',0
.2286:db '������ ᭥������',0
.2287:db '��㯠�� ����',0
.2288:db '��㯠�� ����',0
.2289:db '�ਬ�᭨���',0
.2290:db '��᪮�, ᠬ��',0
.2291:db '祫����',0
.2292:db '�ࠢ����',0
.2293:db '�������',0
.2294:db '祫�����⢮, த 祫����᪨�',0
.2295:db 'ᯮᮡ, �⨫�, �����',0
.2296:db '�㪮����⢮, �ࠢ�筨�, ��筮�',0
.2297:db '�ந�����⢮, ����⮢�����',0
.2298:db '㤮�७��',0
.2299:db '�㪮����',0
.2300:db '������, �����',0
.2301:db '���� (�������᪠�), ����',0
.2302:db '����',0
.2303:db '�ࠬ��',0
.2304:db '����஢���, ���, ������� ���室 ',0
.2305:db '���뫠',0
.2306:db '�ࠩ, ���� (��࠭���), �����',0
.2307:db '���᪮�, 䫮�',0
.2308:db '�業��, �⬥���',0
.2309:db '�뭮�',0
.2310:db '�ࠪ, �����졠, ���㦥�⢮',0
.2311:db '������, ���㦭��',0
.2312:db '��������, ��室��� ����, ��������',0
.2313:db '�����',0
.2314:db '���蠫',0
.2315:db '宧鶴, ��⥫�, �����',0
.2316:db '襤���',0
.2317:db 'ᯨ窠, ஢��, ���, �ࠪ, ���ﭨ�, ����',0
.2318:db '��⥬�⨪�',0
.2319:db '�����',0
.2320:db '�५�, ᮧॢ訩',0
.2321:db '�����',0
.2322:db '���',0
.2323:db '���',0
.2324:db '��',0
.2325:db '���, �࠯���',0
.2326:db '����ॢ�����, ����, ����� �����, ���ࠧ㬥����, ������, �����, �㯮�, �।���, �।���',0
.2327:db '���祭��, ���',0
.2328:db '�।�⢠',0
.2329:db '����ਢ�����',0
.2330:db '���',0
.2331:db '������',0
.2332:db '���訢�����',0
.2333:db '����樭᪨�',0
.2334:db '����樭�, ������⢮',0
.2335:db '��⪨�',0
.2336:db '�������(��), ᮡ������',0
.2337:db '�����, ᮡ࠭��',0
.2338:db '�������',0
.2339:db '���',0
.2340:db '����, ��७. �����, �������(��)',0
.2341:db '童�',0
.2342:db '童��⢮',0
.2343:db '������',0
.2344:db '������',0
.2345:db '��稭�',0
.2346:db '�������, �த����, �������',0
.2347:db '�㯥�, �࣮���',0
.2348:db '�����भ�',0
.2349:db '�����',0
.2350:db '�����न�, �������, �����������',0
.2351:db '⮫쪮, ����',0
.2352:db '��ਤ���',0
.2353:db '���㣠, ���⮨��⢮, ���㦨����',0
.2354:db '��ᥫ�, ࠤ����',0
.2355:db '�������',0
.2356:db '��⠫�',0
.2357:db '��⥮�',0
.2358:db '���稪',0
.2359:db '��⮤, ᯮᮡ, ��⥬�',0
.2360:db '����, ࠧ��� (���)',0
.2361:db '�������',0
.2362:db '�।���',0
.2363:db '�������',0
.2364:db '����ઠ',0
.2365:db '������⢮, ᨫ�',0
.2366:db '������⢥���',0
.2367:db '����',0
.2368:db '����ﭨ� � �����, �᫮ �ன������ ����',0
.2369:db '������, ����᪨�',0
.2370:db '������, �����',0
.2371:db '���ઠ',0
.2372:db '䠡ਪ�, ���쭨�',0
.2373:db '���쭨�',0
.2374:db '���, �襭�',0
.2375:db '�������',0
.2376:db '���������',0
.2377:db '����',0
.2378:db '����ࠫ, ����ࠫ��',0
.2379:db '������',0
.2380:db '�������, ��᫠����, ��饭���',0
.2381:db '��������⢮',0
.2382:db '����⥯����, ����訩, ��. ������, ��ᮢ��襭����⭨�, �����⮪',0
.2383:db '����設�⢮',0
.2384:db '���, ������ ����',0
.2385:db '�����',0
.2386:db '�����, ��⮪��(�), �����, ���஡��, ��⠫��',0
.2387:db '�㤮',0
.2388:db '��ઠ��',0
.2389:db '��',0
.2390:db '�������, ������',0
.2391:db '�������, ����',0
.2392:db '�������',0
.2393:db '�訡��',0
.2394:db '�訡�����, �� �ࠢ��쭮 ������',0
.2395:db 'ᬥ蠭��',0
.2396:db 'ᮢ६����',0
.2397:db '�஬��',0
.2398:db 'த����, ���',0
.2399:db '���죨',0
.2400:db '������� ��ॢ��',0
.2401:db '���죨',0
.2402:db '�����',0
.2403:db '����⭨�',0
.2404:db '����஥���, �ࠬ. ����������',0
.2405:db '�㭠',0
.2406:db '�����',0
.2407:db '���',0
.2408:db '�������訩, ��������',0
.2409:db '����, ���뫥�',0
.2410:db '����',0
.2411:db 'த��� ��',0
.2412:db 'த���',0
.2413:db '�����⥫�, ����',0
.2414:db '��⮬�����',0
.2415:db '���',0
.2416:db '����',0
.2417:db '��',0
.2418:db '���������, ��॥�����',0
.2419:db '��������',0
.2420:db '�����, �祭�',0
.2421:db '�㧥�',0
.2422:db '�ਡ',0
.2423:db '��몠',0
.2424:db '��몠��',0
.2425:db '������',0
.2426:db '���, ���, ���, ���',0
.2427:db 'ᥡ�, ᠬ(�)',0
.2428:db '���, 䠬����',0
.2429:db '������, � ����',0
.2430:db '㧪��',0
.2431:db '����, ��த',0
.2432:db '�ᥭ�த��',0
.2433:db '��樮�����',0
.2434:db '���㤠��⢥��� ����',0
.2435:db '��த��������',0
.2436:db 'த���',0
.2437:db '����⢥���, ��த��',0
.2438:db '����筮',0
.2439:db '��த�',0
.2440:db '�����, �������',0
.2441:db '��',0
.2442:db '����������, ���ॡ�����, �㦤�',0
.2443:db '�ᥤ���',0
.2444:db '���',0
.2445:db '�⪠, ����, ���� (� ���)',0
.2446:db '�������',0
.2447:db '����, ᢥ���',0
.2448:db '����� �ਡ�訩',0
.2449:db '�����',0
.2450:db '᫥���騩 (�� ���浪�), ������訩',0
.2451:db '��� (���祥)',0
.2452:db '��訩, ᨬ����_�',0
.2453:db '������, ����. ����� 5 業⮢',0
.2454:db '����',0
.2455:db '��謠�',0
.2456:db '������',0
.2457:db '������',0
.2458:db '����',0
.2459:db '���',0
.2460:db '�室 ����饭',0
.2461:db '�����த��, ᫠���',0
.2462:db '����ﭨ�',0
.2463:db '����',0
.2464:db '������ �������, �६���, �����',0
.2465:db '��',0
.2466:db '����㬭�',0
.2467:db '�㬭�',0
.2468:db 'ᥢ��',0
.2469:db '���',0
.2470:db '������',0
.2471:db '��, ���, ��',0
.2472:db '���� ���, �������� (� �⢥� �� �������୮���)',0
.2473:db '�⬥���',0
.2474:db '����, ��祣�',0
.2475:db '����饭��, �।�०�����, ����⪠, ��������, �������',0
.2476:db '஬��',0
.2477:db '⥯���',0
.2478:db '� �����饥 �६�',0
.2479:db '�����, ����',0
.2480:db '拉��',0
.2481:db '�������, �楯�����',0
.2482:db '�᫮, ������⢮, �����, �����, ����஢���, �����뢠��',0
.2483:db '�������',0
.2484:db '���',0
.2485:db '���⥫��',0
.2486:db '��',0
.2487:db '�㡮��',0
.2488:db '��᫮',0
.2489:db '����',0
.2490:db '���⢠, ���',0
.2491:db '���ﭪ�',0
.2492:db '����',0
.2493:db '����蠭��',0
.2494:db '������',0
.2495:db '�������, ������������',0
.2496:db '��ꥪ�, 楫�',0
.2497:db '���ࠦ����',0
.2498:db '��易⥫��⢮, ��易������',0
.2499:db '���⠢���, �������',0
.2500:db '�����',0
.2501:db '����筮',0
.2502:db '�� �������騩 ����襭��',0
.2503:db '���',0
.2504:db '����',0
.2505:db '���',0
.2506:db '���⥫쭮� ��᫮, �����, ᬠ�뢠��',0
.2507:db '�������',0
.2508:db '���ﭠ� ��᪠',0
.2509:db '����',0
.2510:db '��',0
.2511:db '�� � ���� ��砥',0
.2512:db '�� �஭�',0
.2513:db '� �।���',0
.2514:db '������',0
.2515:db '��誮�',0
.2516:db '�� � ���� ��砥',0
.2517:db '�� ��࠭��',0
.2518:db '����஦�',0
.2519:db '�� ���㭥',0
.2520:db '�����',0
.2521:db '���ࠢ�',0
.2522:db '�ࠢ� ��',0
.2523:db '����',0
.2524:db '��',0
.2525:db '⮫쪮, �����⢥���',0
.2526:db '����',0
.2527:db '���뢠��, ࠧ���稢���',0
.2528:db '����⢮����',0
.2529:db '����⨢',0
.2530:db '���',0
.2531:db '����',0
.2532:db '���⮢� ᠤ',0
.2533:db '�ਪ��, ���冷�',0
.2534:db '�㤠',0
.2535:db '�࣠�',0
.2536:db '�࣠���᪨�',0
.2537:db '�࣠������',0
.2538:db '�࣠����',0
.2539:db '�࣠������',0
.2540:db '�࣠�����뢠��',0
.2541:db '�࣠��������',0
.2542:db '�࣠������',0
.2543:db '������',0
.2544:db '���筨�, ��砫�, �ந�宦�����',0
.2545:db '��ࢮ��砫��, ��������, �ਣ������, ���������',0
.2546:db '���������, �ந�室���',0
.2547:db '�ୠ����, ��襭��, �����',0
.2548:db '��㣮�, ����',0
.2549:db '����, � ��⨢��� ��砥',0
.2550:db '���',0
.2551:db 'ᥡ�, ᠬ�',0
.2552:db '��, ���, �����',0
.2553:db '���஥��� 䠫�訢�, ����ࠨ����',0
.2554:db '����, ���誠, ��砫� (����� � �. �.)',0
.2555:db '१����, ��室',0
.2556:db '�ਪ, �����',0
.2557:db '���譨�, ���㦭�',0
.2558:db '���㦠�饥 ����࠭�⢮, ��ᬮ�',0
.2559:db '���㦭�� ��஭�, ᭠�㦨',0
.2560:db '������',0
.2561:db '����',0
.2562:db '��, �१, ���, ���',0
.2563:db '�����',0
.2564:db 'ᮡ�⢥���',0
.2565:db '᢮� ����',0
.2566:db '��᫮த',0
.2567:db '�����',0
.2568:db '���, ����',0
.2569:db '�����, ᢥ�⮪',0
.2570:db '����, �������',0
.2571:db '��࠭��',0
.2572:db '��������',0
.2573:db '��᪠, �����, ����� ��᪠��',0
.2574:db '��������',0
.2575:db '��������, ���⨭�',0
.2576:db '���, ��',0
.2577:db 'ࠧ�. ���⥫�, ⮢���, ��㦮�',0
.2578:db '�����',0
.2579:db '������, ��������',0
.2580:db '������, ���쬠',0
.2581:db '�����, ���䫥�',0
.2582:db '����',0
.2583:db '������� �⥪��',0
.2584:db '�㬠��',0
.2585:db '�㬠���� 䠡ਪ�',0
.2586:db '��ࠤ',0
.2587:db '��ࠣ��',0
.2588:db 'த�⥫�',0
.2589:db 'த�⥫�',0
.2590:db '���, ���⠢��� �� ��ﭪ� (��⮬�設�)',0
.2591:db '��ૠ����',0
.2592:db '���⨭��',0
.2593:db '���㣠�',0
.2594:db '���. �����誠',0
.2595:db '����, ���⨥, ஫�, ����. ������, ������(��), ���⠢�����',0
.2596:db '�����, ��������',0
.2597:db '���⭨�',0
.2598:db '�ਭ����� ���⢨�, ���⢮����',0
.2599:db '�ࠬ. ����⨥',0
.2600:db '�����, ���ਭ��',0
.2601:db '��室���, �஥�����, ��������, ᤠ�� (�����), ��।�����, ��ॢ��, �饫�, �ய��',0
.2602:db '��室, �஥��, ��ਤ��, ���뢮� (�� �����)',0
.2603:db '���ᠦ��',0
.2604:db '��宦��',0
.2605:db '��諮�, ����',0
.2606:db '��஦��, �ய����, ����',0
.2607:db '�ண�⥫��',0
.2608:db '�௥���',0
.2609:db '�����, ���쭮�',0
.2610:db '���ਮ�',0
.2611:db '����⢥���',0
.2612:db '��ࠧ��, ������, ��ன��, 㧮�',0
.2613:db '��㧠, ��।�誠, ��⠭��������(��), ������ ����',0
.2614:db '������',0
.2615:db '����',0
.2616:db '���',0
.2617:db '����',0
.2618:db '���ᨪ',0
.2619:db '������',0
.2620:db '���, ���設�',0
.2621:db '����ﭮ� �१',0
.2622:db '����',0
.2623:db '�����',0
.2624:db '�����ﭨ�',0
.2625:db '���',0
.2626:db '���쪠',0
.2627:db '�������',0
.2628:db '���, ��窠',0
.2629:db '�ᥢ�����',0
.2630:db '���������, ����',0
.2631:db '�����',0
.2632:db '��࠭���',0
.2633:db '�����',0
.2634:db '�஭����� ������',0
.2635:db '��窠',0
.2636:db '������஢',0
.2637:db '���ᨮ���',0
.2638:db '�, ��த',0
.2639:db '��த�',0
.2640:db '�⫨��, ����祭��, �ᮢ��襭�⢮����, ������',0
.2641:db '�믮�����',0
.2642:db '��ਮ�',0
.2643:db '�������',0
.2644:db '���ᮭ����, ����',0
.2645:db '�⮣���',0
.2646:db '䨧������',0
.2647:db '䨧���',0
.2648:db '��⥯����, ���',0
.2649:db 'ᮡ����',0
.2650:db '���������',0
.2651:db '���⨭�',0
.2652:db '��ண',0
.2653:db '��᮪',0
.2654:db 'ᢨ���',0
.2655:db 'ᢨ���ଠ',0
.2656:db '�����',0
.2657:db '�㪠',0
.2658:db '���, ��㤠, �⠡���, ���஬������',0
.2659:db '���������⢮',0
.2660:db '�����',0
.2661:db '�㫠���, �ਪ��뢠��',0
.2662:db '�������, ����',0
.2663:db '���, ��থ��',0
.2664:db '����',0
.2665:db '���, ���⮩, �������, ࠢ����',0
.2666:db '�������, ���뢭�',0
.2667:db '���',0
.2668:db '����, �奬�, ����',0
.2669:db '���᪮���, ᠬ����, �㡠���',0
.2670:db '������',0
.2671:db '��᪠',0
.2672:db '��⥭��, �����, 䠡ਪ�',0
.2673:db '�������',0
.2674:db '��������, ��㪠��ઠ',0
.2675:db '�५��, ��᪠',0
.2676:db '�����ଠ, �ਡ㭠',0
.2677:db '���, ���',0
.2678:db '����� � �嬠��',0
.2679:db '����� � ᭥���',0
.2680:db '����� �� �������',0
.2681:db '��ப, ����, ��몠��',0
.2682:db '��஢�� ���頤��, ᯮ�⨢��� ���頤��',0
.2683:db '������ ���� (� �㤥), ��ࠢ�뢠����',0
.2684:db '��������',0
.2685:db '㤮�����⢨�',0
.2686:db '���⮪ �����, ���ﭪ�, ��䨪',0
.2687:db 'ᬥ��, �⢠���',0
.2688:db '᫨��',0
.2689:db '����, �����',0
.2690:db '�����, �ࠡ���',0
.2691:db '����',0
.2692:db '���������, ��ᯠ����� ������',0
.2693:db '��ଠ�',0
.2694:db '���, ���',0
.2695:db '����',0
.2696:db '�窠, �㭪�, �����, �窮, ���ਥ, �����뢠�� (����楬)',0
.2697:db '�窠 �७��',0
.2698:db '㪠�뢠��',0
.2699:db '�����, ���������⥫��, ᮢ��襭�� �祢����',0
.2700:db '㪠���, 㪠��⥫�',0
.2701:db '�, ��ࠢ����',0
.2702:db '冷����',0
.2703:db '�������, ����',0
.2704:db '������᪨�',0
.2705:db '������᪨� ���⥫�',0
.2706:db '����⨪�',0
.2707:db '������ ����ᮢ',0
.2708:db '��譮���, �����',0
.2709:db '���',0
.2710:db '����',0
.2711:db '�㦠',0
.2712:db '�����, ������, ���宩',0
.2713:db '���� ਬ᪨�',0
.2714:db '⮯���',0
.2715:db '���',0
.2716:db '�������',0
.2717:db '���',0
.2718:db '᭨����',0
.2719:db '(���ﭠ�) ���',0
.2720:db '����, ������',0
.2721:db '��ᨫ�騪, 袥���',0
.2722:db '����, ����',0
.2723:db '������',0
.2724:db '������',0
.2725:db '��������',0
.2726:db '������, ������, ���',0
.2727:db '���⠫쮭',0
.2728:db '����䥫�',0
.2729:db '������� ���',0
.2730:db '����ଠ',0
.2731:db '����',0
.2732:db '��魮���, ������',0
.2733:db '����, ᨫ��',0
.2734:db '�������稥',0
.2735:db '�ࠪ��᪨',0
.2736:db '�ய���������',0
.2737:db '��砩��, ���������',0
.2738:db '�।���஦�����',0
.2739:db '�।��⢮����',0
.2740:db '��楤���',0
.2741:db '�।�����騩',0
.2742:db '�ࠣ�業��',0
.2743:db '�ய����, ����',0
.2744:db '���',0
.2745:db '�筮���',0
.2746:db '�।��⢥����, �।��',0
.2747:db '�।������',0
.2748:db '��⮢���',0
.2749:db '�।��ᠭ��, �楯�',0
.2750:db '������⢨�',0
.2751:db '������⢮����, ����ப',0
.2752:db '�룫�����',0
.2753:db '��ᨢ�',0
.2754:db '�८������騩',0
.2755:db '�।������, �����',0
.2756:db '�।���饭��, �।�०�����',0
.2757:db '�।��騩, �।�����騩',0
.2758:db '��',0
.2759:db '業�',0
.2760:db '���業��',0
.2761:db '(�)������(��)',0
.2762:db '����稩',0
.2763:db '��म���',0
.2764:db '��饭���',0
.2765:db '(��ࢮ)��砫��, ��ࢨ��, �᭮����',0
.2766:db '�ਢ���, ����',0
.2767:db '�ਧ, �६��',0
.2768:db '����⭮���',0
.2769:db '����⭮',0
.2770:db '�஡����, �����',0
.2771:db '�த������, �ந�室���, ����㯠��, ���室��� (� 祬�-�.)',0
.2772:db '����㯮�, ��ய��⨥, ��⮪���, ����᪨',0
.2773:db '�����',0
.2774:db '������',0
.2775:db '�஢��������, ������',0
.2776:db '���������, �ப������',0
.2777:db '���⠢���, ���뢠��',0
.2778:db '�ந�������, �।����, �த���, �த��',0
.2779:db '���⠭��騪',0
.2780:db '�த��, १����, �����',0
.2781:db '�ந�����⢮, �த���',0
.2782:db '������',0
.2783:db '����ᨮ�����, ����ᨮ���, ᯥ樠����',0
.2784:db '������',0
.2785:db '�룮��, ���짠, �ਡ��, �ਭ���� �����, ��������� �����',0
.2786:db '�ਡ���, �룮���, �������',0
.2787:db '��㡮���',0
.2788:db '�ணࠬ��',0
.2789:db '�ணࠬ��',0
.2790:db '�ணࠬ��஢����',0
.2791:db '�ண���',0
.2792:db '�ண��ᨢ��',0
.2793:db '������',0
.2794:db '�����ઠ, ����, ��������',0
.2795:db '�ய������',0
.2796:db '᢮��⢥���, �ࠢ����, �������騩',0
.2797:db '�ࠬ. ��� ᮡ�⢥����',0
.2798:db '�ࠬ. ��� ᮡ�⢥����',0
.2799:db '��� ᫥���',0
.2800:db 'ᮡ�⢥������, �����⢮, ᢮��⢮',0
.2801:db '����',0
.2802:db '�����뢠��, ����뢠���� (���-�., 祬-�.)',0
.2803:db '��᫮���',0
.2804:db '᭠�����, ���ᯥ稢���',0
.2805:db '�᫨, �� �᫮���',0
.2806:db '�஢������',0
.2807:db '�஢����, �������, ��� ���⥫쭮��',0
.2808:db '�஢�����',0
.2809:db '��뢠�騩, �஢���樮���',0
.2810:db '��뢠��, �஢��஢���',0
.2811:db '���஦��, �����ࠧ㬭�',0
.2812:db '�����',0
.2813:db '����⢥���, �ࠦ���᪨�, ��த��',0
.2814:db '��㡫��������, �㡫�����, �������',0
.2815:db '���᭮���, ४����',0
.2816:db '�㡫�������',0
.2817:db '�㤨��',0
.2818:db '�����',0
.2819:db '�祭��',0
.2820:db '��ਮ��⪠',0
.2821:db '饭��',0
.2822:db '���㯠��, ���㯪�',0
.2823:db '����, ���ਬ���, �������, ��⥩訩',0
.2824:db '��⥩�� ��㬪�',0
.2825:db '��⪠',0
.2826:db '����, �����筮���',0
.2827:db '������, �������',0
.2828:db '��襫��, ��७. ���죨',0
.2829:db '��᫥������',0
.2830:db '������, �⠢���',0
.2831:db '�����뢠��',0
.2832:db '��⠢����',0
.2833:db '�⪫��뢠��',0
.2834:db '��������',0
.2835:db '�����',0
.2836:db '�����䨪���, �����ઠ, ��࠭�祭��, ᢮��⢮, ����⢮',0
.2837:db '����⢮, ���⮨��⢮, ᢮��⢮',0
.2838:db '������⢮',0
.2839:db '���',0
.2840:db '�����������, �����',0
.2841:db '�⢥���, ����⠫ (����), ����. ����� � 25 業⮢',0
.2842:db '����०���',0
.2843:db '��஫���, ��. ���',0
.2844:db '��࠭��, ��業����',0
.2845:db '�⮫��� (�����)',0
.2846:db '���᪨',0
.2847:db '�����, �������� �����(�), ��訢���, �����࣠�� ᮬ�����',0
.2848:db '����᭨�, �����',0
.2849:db '��।�, ����� � ��।�',0
.2850:db '������',0
.2851:db '�����, �஢�୮',0
.2852:db '�����',0
.2853:db '�����, �設�',0
.2854:db '�����',0
.2855:db '�⥣���� ���﫮',0
.2856:db '��⠢����, ��������',0
.2857:db 'ᮢ��襭��, ������, ��楫�',0
.2858:db '�஦���, �९����',0
.2859:db '���',0
.2860:db '����窨',0
.2861:db '��஢���, ��뫠����',0
.2862:db '�஫��',0
.2863:db '�����, ��',0
.2864:db '�ᮢ�',0
.2865:db '������, ���騩, ��祧���',0
.2866:db 'ࠤ����',0
.2867:db '��७���, �᭮����, ࠤ������, ࠤ����',0
.2868:db 'ࠤ��',0
.2869:db '।�᪠',0
.2870:db '����, ��஬',0
.2871:db '��ﯪ�',0
.2872:db '�����, �������, ��襢���',0
.2873:db '���९����, ���ࢠ���, ࢠ��',0
.2874:db '�����',0
.2875:db '��ਫ�, ��४������, ५��',0
.2876:db '��ࠤ�, ��ਫ�',0
.2877:db '����. �������� ��ண�',0
.2878:db '�������� ��ண�',0
.2879:db '���������஦�� ������',0
.2880:db '���������஦���',0
.2881:db '�����',0
.2882:db 'ࠤ㣠',0
.2883:db '��������, ����',0
.2884:db '��������',0
.2885:db '���������',0
.2886:db '����㦤���, �������',0
.2887:db '������� �����, ��ᯨ�뢠��, ���騢���, ࠧ����ਢ���, ����. ����襭�� (��௫���)',0
.2888:db '���',0
.2889:db '�ࠡ��, �ॡ���',0
.2890:db '��࠭',0
.2891:db '������',0
.2892:db '(��)�������',0
.2893:db '����',0
.2894:db '����',0
.2895:db '�樮�����, ࠧ㬭�',0
.2896:db '���',0
.2897:db '���⨣���',0
.2898:db '����',0
.2899:db '��⥫�',0
.2900:db '� ��⮢������',0
.2901:db '�⥭��',0
.2902:db '�⠫�� ���',0
.2903:db '��⮢�',0
.2904:db '�����騩',0
.2905:db '�।�⠢��� ᥡ�, �믮�����',0
.2906:db '����⢨⥫쭮',0
.2907:db '��뢠��, �⬥����, �ᯮ������',0
.2908:db '����祭��, �ᯨ᪠, ���⠭��',0
.2909:db '�������, �ਭ����� (���⥩)',0
.2910:db '�����⥫�, ��. �ਥ����, ��㡪� (⥫�䮭���)',0
.2911:db '��������, ����, ᢥ���',0
.2912:db '�������',0
.2913:db '�ਥ�, �����⨥',0
.2914:db '����, �������஢���',0
.2915:db '���ﭭ�, ������㤭�, ��஬��稢�',0
.2916:db '�����, ������뢠��',0
.2917:db '����뢠�� ��',0
.2918:db '�ਧ�����',0
.2919:db '�ਧ������, 㧭�����',0
.2920:db '�ਯ�������',0
.2921:db '��ᯮ�������',0
.2922:db '४����������, ᮢ�⮢���',0
.2923:db '४��������',0
.2924:db '������ࠦ����, �������஢���',0
.2925:db '�ਬ�����',0
.2926:db 'ࠧ�����',0
.2927:db '४������஢���',0
.2928:db '४��������',0
.2929:db '�����뢠��, ॣ����஢���',0
.2930:db '���㦭�� ᯨ᮪',0
.2931:db '��������, �매�ࠢ������, ���ࠢ������',0
.2932:db '�매�஢�����, ���ࠢ�����',0
.2933:db '�����࠭��, ४���, ��࡮����',0
.2934:db '���������� (� 祬�-�.), ����������',0
.2935:db '����',0
.2936:db '��㯠��, ��㯠��, �믮����� (���頭�� � �. �.)',0
.2937:db '㬥�����, ��������',0
.2938:db '᪨���, 㬥��襭��',0
.2939:db '��뫠���� ��, ���ࠢ���� ����-�.',0
.2940:db '�ࠢ��, ��뫪�, 㯮�������',0
.2941:db '��ࠦ���(��), ࠧ������',0
.2942:db '��ࠦ����, ࠧ��諥���',0
.2943:db '��ନ஢���, ��ࠢ����(��), ��ଠ',0
.2944:db '�����ন������',0
.2945:db '�����ন������ ��',0
.2946:db '�ᢥ����',0
.2947:db '����९����� (ᨫ � �. �.)',0
.2948:db '����',0
.2949:db '宫����쭨�, ��०����',0
.2950:db '�������, ���',0
.2951:db '�������, �����',0
.2952:db '��������',0
.2953:db '�⢥࣠��',0
.2954:db 'ࠤ�����(��)',0
.2955:db '��᪠�뢠��',0
.2956:db '�⭮襭��, த�⢥����, �����⢮�����, ���������',0
.2957:db 'த�⢮, (������-) �⭮襭��, ���',0
.2958:db '�⭮�⥫��, �ࠢ��⥫��, த�⢥����',0
.2959:db '�᫠�����, �����, ������, ������ ��।���',0
.2960:db '�᢮�������, ���᪠��, ���᪠��, �᢮��������',0
.2961:db '�������',0
.2962:db '����������',0
.2963:db '�������',0
.2964:db '����ਥ, 㢥७�����',0
.2965:db '�����祭��, ������, ��ᮡ��, ᬥ�� (������� � �. �.)',0
.2966:db '५����',0
.2967:db '५�������',0
.2968:db '��������, ����뢠�� ������, �᢮�������, ᬥ����',0
.2969:db '���ᯮ�������',0
.2970:db '����⭮',0
.2971:db '����������',0
.2972:db '���������� ��',0
.2973:db '��⠢�����, ���⪨, ��⠭��',0
.2974:db '���⮪',0
.2975:db '�������, ����砭��',0
.2976:db '�����⥫��',0
.2977:db '�।�⢮, ������⢮, ��ࠢ����',0
.2978:db '�������, �ᯮ������',0
.2979:db '��ᯮ�������, ������',0
.2980:db '����������',0
.2981:db '��ᯮ�������',0
.2982:db '���⮪',0
.2983:db '��맥��� ᮢ���',0
.2984:db '�⤠�����, 㥤������',0
.2985:db '㤠�����, ���࠭����, ��॥�� �� ����� �������',0
.2986:db '㤠����, ���࠭���, ᭨����, ��॥�����',0
.2987:db '������������',0
.2988:db '�७���� ����, (�������� | ᤠ����) � �७��',0
.2989:db '��������',0
.2990:db '�������� ���⭮, ��������, �������',0
.2991:db '�⢥���, �⢥�',0
.2992:db '������, ����ᥭ��, ࠯���, ����, ��� ����५�, ᮮ����, ������뢠��',0
.2993:db '������稪, ९����',0
.2994:db '��㡫���',0
.2995:db '९����',0
.2996:db '���졠, �����',0
.2997:db '�㦤����� (� 祬-�.), �ॡ�����',0
.2998:db 'ᯠ���, ᯠᥭ��',0
.2999:db '��᫥�������',0
.3000:db '������, �������(��), �६���, �ந������� ᥭ���',0
.3001:db '㢠�����, 㢠����',0
.3002:db '���⥭��, ���冷��',0
.3003:db '����⥫��',0
.3004:db 'ᮮ⢥��⢥���',0
.3005:db '��।�誠, ����窠',0
.3006:db '�⢥���, ��뢠����',0
.3007:db '�⢥�, �⪫��',0
.3008:db '�⢥��⢥������, ��易������',0
.3009:db '����',0
.3010:db '१����',0
.3011:db 'ॢ�����, ��ॢ���',0
.3012:db 'ॡ�',0
.3013:db '����',0
.3014:db '������, �����த��, ���� (� ���)',0
.3015:db '������⢮',0
.3016:db '����',0
.3017:db '�ࠢ�',0
.3018:db '�������',0
.3019:db 'ᯥ��',0
.3020:db '�����������',0
.3021:db '��, �᪮����',0
.3022:db 'ᮯ�୨�, �����७�',0
.3023:db '४�',0
.3024:db '�������',0
.3025:db '����������',0
.3026:db '��ண�',0
.3027:db '�த���, ᪨�����',0
.3028:db 'ॢ���, ॢ, ���',0
.3029:db '�����(��), ��७�, ��મ�',0
.3030:db '�ࠡ���, ����஢뢠��',0
.3031:db '�ࠡ�⥫�, ࠧ������',0
.3032:db '�ࠦ�, �ࠡ��',0
.3033:db '���������',0
.3034:db '᪠��, ������, �����(��), 㡠����',0
.3035:db '������ ���宬',0
.3036:db '஫�',0
.3037:db '�����(��), ᢥ��뢠��(��), �᪠�뢠�� (���), ᢥ�⮪, ᯨ᮪, �㫮�, �㫮窠, ��窠',0
.3038:db '��४��窠',0
.3039:db '��⮪, �����',0
.3040:db '஬��⨪�, ஬����᪠� �����',0
.3041:db '஬����᪨�, ஬�����',0
.3042:db '����, �஢, ���⨫��� �����',0
.3043:db '���, ��. �����',0
.3044:db '������, ����, ����࠭�⢮',0
.3045:db '�����, ��ॢ��',0
.3046:db '஧�, ���������',0
.3047:db '����',0
.3048:db '��㣫�',0
.3049:db '�㤨��, ���㦤���',0
.3050:db '�������',0
.3051:db '��',0
.3052:db '��������, �㨭�',0
.3053:db '�������',0
.3054:db '������',0
.3055:db '㡥����',0
.3056:db '����',0
.3057:db '��� (��� ᯮ��)',0
.3058:db 'ᮡ���, ᮡ���� ���',0
.3059:db '��讪',0
.3060:db '��饭��, ���ਪ�᭮�����',0
.3061:db '���⢮�ਭ�襭��, ���⢠',0
.3062:db '��砫��',0
.3063:db 'ᥤ��',0
.3064:db '���।���, ��������, ᥩ�',0
.3065:db '楫 � ���।��',0
.3066:db '��࠭��, �।���஦�����, ��࠭���',0
.3067:db '������᭮���',0
.3068:db '�������, ᪠����',0
.3069:db '�����, ������� (�� ��ࠡ��)',0
.3070:db '����',0
.3071:db '��⮩',0
.3072:db 'ࠤ�',0
.3073:db 'ᠫ��',0
.3074:db '���������, �����',0
.3075:db '�த���, ��த���',0
.3076:db '�த����',0
.3077:db '�����, ᥬ��',0
.3078:db '᮫�, ᮫���',0
.3079:db '�ਢ���⢨�, ᠫ��',0
.3080:db '�� ��, ���������',0
.3081:db '��ࠧ��',0
.3082:db 'ᠭ���, ࠧ�襭��',0
.3083:db '��᮪',0
.3084:db 'ᠭ����, ����த',0
.3085:db '��ଠ���, � ��ࠢ�� 㬥',0
.3086:db 'ᯥ��',0
.3087:db 'ᠭ����, ��������᪨�',0
.3088:db '���⨫��, ����㧨���',0
.3089:db 'ᨤ���',0
.3090:db 'ᯠ���',0
.3091:db '������, ᬮ����',0
.3092:db '�������, ᪠����',0
.3093:db '���, ������',0
.3094:db '誮��',0
.3095:db '誮�쭠� �ଠ',0
.3096:db '���᪭�� 誮��',0
.3097:db '誮�쭨�, �祭��',0
.3098:db '誮�쭨��, �祭���',0
.3099:db '誮�쭨�, �祭��',0
.3100:db '誮��� ����',0
.3101:db '��㪠',0
.3102:db '�祭�',0
.3103:db '��㣮���, �����, ࠧ���',0
.3104:db '���',0
.3105:db 'ࠧ���稪, ᪠��',0
.3106:db '�ଠ, �࠭, ����. �����, ����ࠦ�����, ������, ��뢠��, ��������஢��� �� �࠭�',0
.3107:db '�����',0
.3108:db '���, �����',0
.3109:db '���',0
.3110:db '砩��',0
.3111:db '���, �����, ������',0
.3112:db '订',0
.3113:db '���᪮� ����०�',0
.3114:db 'ᥧ��, ��� ����',0
.3115:db '���, ᨤ���, ����',0
.3116:db 'ᥪ㭤�, ��ன',0
.3117:db '�।���',0
.3118:db 'ᥪ��, ⠩��',0
.3119:db 'ᥪ��, �⤥�',0
.3120:db '������, ᬮ����',0
.3121:db '�஢����� (㥦��饣�)',0
.3122:db 'ᥬ�',0
.3123:db '।��',0
.3124:db '�롨���, ���࠭��, �⡮��',0
.3125:db '�롮�',0
.3126:db '�த�����',0
.3127:db '���뫠��',0
.3128:db '���訩',0
.3129:db '���襪��᭨�',0
.3130:db '���饭��, ���⢮, ᥭ���',0
.3131:db '���⢮, ᮧ�����, ���, ���⢮����',0
.3132:db '�����᫥���, ������⢥���',0
.3133:db '��᫠��',0
.3134:db '�।�������',0
.3135:db '��쥧��',0
.3136:db '�㦨��',0
.3137:db '�㣨',0
.3138:db '����, ��ᥤ����',0
.3139:db '�⠢���',0
.3140:db 'ᥬ�',0
.3141:db 'ᥤ쬮�',0
.3142:db '��᪮�쪮',0
.3143:db '���',0
.3144:db '���',0
.3145:db '���ઠ',0
.3146:db '������, ���� (�㬠��, ������)',0
.3147:db '�����',0
.3148:db 'ᢥ���',0
.3149:db '��ࠡ��, �㤭�',0
.3150:db '����㧪�, ���',0
.3151:db '��ࠡ�����襭��',0
.3152:db '�����',0
.3153:db '��᪠� �㡠誠',0
.3154:db '��謠�, ����',0
.3155:db 'ᢥ���, �஫�����',0
.3156:db '��५���',0
.3157:db '�������',0
.3158:db '�த����',0
.3159:db '������ ������� ��� ���㯮�, ���㯠��',0
.3160:db '宧��⢥���� �㬪�',0
.3161:db '��ॣ',0
.3162:db '���⪨�',0
.3163:db '����५���',0
.3164:db '�����',0
.3165:db '�����뢠��',0
.3166:db '��������',0
.3167:db '�஭��⥫��, ������ (�����)',0
.3168:db '�����, ��ਪ����, �஭��⥫�� �ਪ',0
.3169:db '१���, �஭��⥫��',0
.3170:db '�������, ᠤ����� (� ���ਨ)',0
.3171:db '����뢠��',0
.3172:db '�������, �譮�, ࢮ�',0
.3173:db '��஭�, ���',0
.3174:db '�冷�',0
.3175:db '����. �����',0
.3176:db '�����, ��ᢥ���',0
.3177:db '�ᠤ�',0
.3178:db '���, ��',0
.3179:db '�������, �����',0
.3180:db '�७��',0
.3181:db '���⮯ਬ��⥫쭮��',0
.3182:db '�ᬮ�� ���⮯ਬ��⥫쭮�⥩',0
.3183:db '����, �ਧ���, �ᯨ�뢠����, ������뢠��(��)',0
.3184:db 'ᨣ���, ����, ᨣ������஢���',0
.3185:db '�������',0
.3186:db '�뢥᪠',0
.3187:db '���祭��',0
.3188:db '�����, ����⢥���',0
.3189:db '������, �������, ����� ���祭��',0
.3190:db '���砭��, �設�, ���⠢��� ������, ��������',0
.3191:db '���������, ���砭��',0
.3192:db '��',0
.3193:db '����������',0
.3194:db '����',0
.3195:db '�ॡ�, �ॡ���',0
.3196:db '���⮩',0
.3197:db '�����६����',0
.3198:db '���, �����',0
.3199:db '� ⮣� �६��� ���, � �� ���',0
.3200:db '����',0
.3201:db '�����',0
.3202:db '�����⢥���',0
.3203:db '���',0
.3204:db '����',0
.3205:db 'ᨤ���, ��ᥤ���',0
.3206:db '�ᯮ�������',0
.3207:db '����',0
.3208:db '��⮩',0
.3209:db 'ࠧ���, ����稭�',0
.3210:db '������� �� ���쪠�',0
.3211:db '�릠, ������� �� �릠�',0
.3212:db '����, ���, �����',0
.3213:db '��������� �������� � ��᪥',0
.3214:db '�',0
.3215:db '�९',0
.3216:db '����',0
.3217:db 'ᠭ�, ������� �� ᠭ��',0
.3218:db 'ᯠ��',0
.3219:db 'ᯠ��',0
.3220:db '���⨪',0
.3221:db '��������',0
.3222:db '��������',0
.3223:db '�����쪨�',0
.3224:db '�롪�, �론����',0
.3225:db '��',0
.3226:db '᭥�',0
.3227:db '᭥���',0
.3228:db '᭥�����',0
.3229:db '⠪, ⠪�� ��ࠧ��',0
.3230:db '�뫮',0
.3231:db '�樠�����᪨� ��ன',0
.3232:db '��᮪',0
.3233:db '�����',0
.3234:db '�த���',0
.3235:db '�����',0
.3236:db '᮫���',0
.3237:db '����袠, �����⪠, �����⢥���',0
.3238:db '��ꥧ��, �থ�⢥���',0
.3239:db '⢥��, ᯫ�譮�, �९���, ᮫����, �᭮��⥫��',0
.3240:db '��᪮�쪮, �������',0
.3241:db '��-�',0
.3242:db '��-�, ����',0
.3243:db '������',0
.3244:db '��',0
.3245:db '����',0
.3246:db '�᪮�, ᪮�',0
.3247:db 'ᠦ�',0
.3248:db '�ᯮ�������, �����, �������� (����)',0
.3249:db '���⢨⥫��, ����������',0
.3250:db '����祭��',0
.3251:db '�����, � �⢫����',0
.3252:db '���',0
.3253:db '���, ������, ���஢�, �९���, �������� ��㡨��',0
.3254:db '��',0
.3255:db '����',0
.3256:db '���筨�, ��砫�',0
.3257:db '�, �, �� �(�), � ��',0
.3258:db '-���⮪',0
.3259:db '�',0
.3260:db '�㢥���',0
.3261:db '�㢥����',0
.3262:db '�㢥ਭ���',0
.3263:db '����࠭�⢮, ����ﭨ�, ���殮���',0
.3264:db '��ᬮ����',0
.3265:db '��ᬨ�᪨� ��ࠡ��',0
.3266:db '�������',0
.3267:db 'ᯨ���, �����',0
.3268:db 'ᯥ樠���',0
.3269:db '᪮����',0
.3270:db '�룮��ਢ��� �� �㪢��',0
.3271:db '�����',0
.3272:db '�����',0
.3273:db '�����',0
.3274:db '�������',0
.3275:db '�����',0
.3276:db 'ᯮ��, ࠧ���祭��',0
.3277:db 'ᯮ�⨢��� ���頤��',0
.3278:db 'ᯮ��ᬥ�',0
.3279:db '��⭮, ����, ���騪',0
.3280:db '�������',0
.3281:db '��ᨪ (�����), �����',0
.3282:db '��룭���, ����������',0
.3283:db '��᭠',0
.3284:db '������, ���頤�, ᪢��',0
.3285:db '��⮩稢����, �⮩�����, ��筮���',0
.3286:db '��⮩稢�, �⮩���, ����, ������',0
.3287:db '�⮣, ���, ��㤠',0
.3288:db '�⠤���',0
.3289:db '���, ���ᮭ��, ����. �⠡',0
.3290:db '�業�, ���業�஢���, �⠢��� (����), 䠧�, �⠤��',0
.3291:db '������',0
.3292:db '���⮩',0
.3293:db '��. ���',0
.3294:db '�⥡���',0
.3295:db '���⮢�� ��ઠ',0
.3296:db '�����',0
.3297:db '��⠢���',0
.3298:db '������',0
.3299:db '���⠫쭮 ᬮ����, �������, ���⠫�� �����',0
.3300:db '᪢���',0
.3301:db '��稭���, ��ࠢ������, �᪮���, ���ࠣ�����, ��砫�, ᯮ��. ����',0
.3302:db '��㣠��, ��ࠦ���',0
.3303:db '�����, ���饭��',0
.3304:db '��������, 㬨��� �� ������, ����� �������',0
.3305:db '���㤠��⢮, ���, ���ﭨ�, �����, ��㫨஢���',0
.3306:db '�⠭��, ������, ����⢥���� ���������, �⠢���, ࠧ�����',0
.3307:db '�����',0
.3308:db '��⠢�����',0
.3309:db '���⠢���',0
.3310:db '����ᥤ',0
.3311:db '�⠫�, �⠫쭮�',0
.3312:db '���⮩',0
.3313:db '�ࠢ��� �㫥�, �ࠢ���� (��設��), ���ࢫ���',0
.3314:db '�������',0
.3315:db '�⢮�, �⥡���',0
.3316:db '蠣, ��㯥�쪠, ��㯠��, 蠣���',0
.3317:db '���थ��',0
.3318:db '�����, 楯�',0
.3319:db '�娩, ����������, �� �� ���, �� ��, ������',0
.3320:db '�� ����',0
.3321:db '� �宬 ���� ��� �������',0
.3322:db 'த, ��த�, �����, 䮭�, ��樮���� ����⠫',0
.3323:db '䮭����� ��ঠ',0
.3324:db '�㫮�',0
.3325:db '������',0
.3326:db '�����',0
.3327:db '��⠭��������(��), �४����(��), ���몠��, �����뢠��, ������஢��� (��), ��⠭����, ����প�, ���� �९������',0
.3328:db '�஡��, ����窠',0
.3329:db '�࠭����',0
.3330:db '�����, ᪫��, �������',0
.3331:db '�⠦',0
.3332:db '����',0
.3333:db '����, �஧�, ����. ����, ��襢���',0
.3334:db '����',0
.3335:db '��᪠�, �������, �����',0
.3336:db '⮫���, �����, �९���',0
.3337:db '����, ��窠',0
.3338:db '��אַ�',0
.3339:db '���שׂ���(��)',0
.3340:db '�������, ������(��), ����殮���',0
.3341:db '㧪��, �஫��, ����㤭�⥫쭮� ���ਠ�쭮� ���������, �㦤�',0
.3342:db '��࠭��',0
.3343:db '��⠭����, ��⠭����������',0
.3344:db '����',0
.3345:db '�������, ࠧ������(��), �����, ����᪠, �����',0
.3346:db '�����, ��訢��',0
.3347:db '�������',0
.3348:db '�������',0
.3349:db '㤠�, �����, �������, �����������',0
.3350:db '�த���, �ண㫨������, �ண㫪�',0
.3351:db 'ᨫ��',0
.3352:db '����, 㤠������',0
.3353:db '���졠',0
.3354:db '��㤥��',0
.3355:db '������, ����(��)',0
.3356:db '����⢮, ���ਠ�, ��������, ���஢���, ���몠��',0
.3357:db 'ᯮ�몠����, ����������',0
.3358:db '������ ��⪭������',0
.3359:db '����, ���㡮�',0
.3360:db '�������, �襫������',0
.3361:db '����, �㯮�',0
.3362:db '�⨫�, ᫮�, ����, �ᮭ',0
.3363:db '���稭���',0
.3364:db '����������',0
.3365:db '���稭����, ��������, ��������, �।���, ⥬�',0
.3366:db '���稭�����, ����������, �।�⠢���� �� ��ᬮ�७��',0
.3367:db '⮭���, ��㫮����',0
.3368:db '⮭�����',0
.3369:db '������',0
.3370:db '���⠭��',0
.3371:db '�ਣ�த',0
.3372:db '�ਣ�த��',0
.3373:db '⮭���, ���ய���⥭',0
.3374:db '�ᯥ�',0
.3375:db '㤠��, �ᯥ��',0
.3376:db '⠪��',0
.3377:db '����',0
.3378:db '����',0
.3379:db '���',0
.3380:db '�����',0
.3381:db '�㬬�, �⮣',0
.3382:db '�㬬�஢���',0
.3383:db '���',0
.3384:db '᮫����',0
.3385:db '����譨�, �१����',0
.3386:db '㦨�',0
.3387:db '����������, �ਫ������, ���������',0
.3388:db '᭠�����, ���⠢����, ᭠�����, �����, �. �।�������',0
.3389:db '�����ন����, ᮤ�ঠ��, �����প�',0
.3390:db '��஭���, �ਢ�থ���',0
.3391:db '�।��������, ��������',0
.3392:db '��ᮬ�����, 㢥७��',0
.3393:db '�����孮���',0
.3394:db '����, ������ ���',0
.3395:db '���ࣨ�, �ਥ���� (���࣠)',0
.3396:db '䠬����',0
.3397:db '�ॢ��室���',0
.3398:db '����襪, ����譨�',0
.3399:db '�ਡ���筠� �⮨�����',0
.3400:db '������, ����窠',0
.3401:db '�������',0
.3402:db '��������',0
.3403:db '��������',0
.3404:db '�������',0
.3405:db '�������',0
.3406:db '��������',0
.3407:db '���ᥩ�',0
.3408:db '���, 诠��, ᠡ��',0
.3409:db 'ᨬ���',0
.3410:db '����⢮����',0
.3411:db '����⢨�',0
.3412:db 'ᨬ䮭��',0
.3413:db '�ਧ���, ᨬ�⮬',0
.3414:db '䨧. ᨭ��䠧��஭',0
.3415:db '���',0
.3416:db '��⥬�',0
.3417:db '�⮫, ���, ⠡���',0
.3418:db '᪠����',0
.3419:db '���砫���',0
.3420:db '���砫��� 㣮���',0
.3421:db '⠪�',0
.3422:db '⠪���',0
.3423:db '���⠪��',0
.3424:db '墮��',0
.3425:db '���⭮�',0
.3426:db '����',0
.3427:db 'ࠧ����� (�� ���)',0
.3428:db '��ᬠ�ਢ��� ����-�, �������� � ���-�',0
.3429:db '��������, �ਡ�� (��������)',0
.3430:db '᭨����',0
.3431:db '�砢�⢮����',0
.3432:db '����㯠�� �� ᮡ࠭��',0
.3433:db '᪠���',0
.3434:db 'ࠧ�����, ࠧ����ਢ�����',0
.3435:db '��᮪��',0
.3436:db '��쬠, ⥫���䭠� ����',0
.3437:db '������䮭',0
.3438:db '��襭�',0
.3439:db '������',0
.3440:db '�����, �������� �������',0
.3441:db '⠪�',0
.3442:db '砩',0
.3443:db '砩��� �����',0
.3444:db '����',0
.3445:db '��⥫�',0
.3446:db '�������',0
.3447:db '�ࠧ����, �����',0
.3448:db '�孨�᪨�',0
.3449:db '��⥦�',0
.3450:db '�孨��',0
.3451:db '����, �⮬�⥫��',0
.3452:db '�����',0
.3453:db '⥫��ࠬ��',0
.3454:db '⥫����, ⥫����஢���',0
.3455:db '⥫�䮭',0
.3456:db '⥫��������',0
.3457:db 'ᮮ����',0
.3458:db '�����',0
.3459:db '�����ঠ���, 㬥७��',0
.3460:db '⥬������',0
.3461:db '�६����',0
.3462:db '������, ᮡ�������',0
.3463:db '���襭��, ᮡ����',0
.3464:db '������',0
.3465:db '⥭��',0
.3466:db '⥭����',0
.3467:db '����⪠',0
.3468:db '������',0
.3469:db '����, ����',0
.3470:db '㦠��',0
.3471:db '������',0
.3472:db '���, ���⠭��, �஢�ઠ, �஢�����',0
.3473:db '⥪��',0
.3474:db '⥪�⮢�� ����',0
.3475:db '⥪�⨫��',0
.3476:db '����������',0
.3477:db '��������� ���',0
.3478:db '���������',0
.3479:db '���, ��, ��',0
.3480:db '�� � ���浪�',0
.3481:db '�ࠢ��쭮',0
.3482:db '��⮬�',0
.3483:db '�����砭�',0
.3484:db '��� ����稫��� ������',0
.3485:db '�� ����',0
.3486:db '���� � ⮬',0
.3487:db '�ࠧ�� �ࠢ�����',0
.3488:db '�� ������',0
.3489:db '� ��� ⮣� �����',0
.3490:db '���ய���⥭ (� �������)',0
.3491:db '(��) ᠬ�',0
.3492:db '⥠��',0
.3493:db '�孨�',0
.3494:db '��, ��',0
.3495:db 'ᥡ�, ᠬ�',0
.3496:db '⮣��, ��⥬, ��⮬',0
.3497:db '⥮��',0
.3498:db '⠬',0
.3499:db '�� �뫮 ��㣮�� ��室�',0
.3500:db '��',0
.3501:db '���',0
.3502:db '��� �祭� ��宦� ��� �� ��㣠',0
.3503:db '��� ��������� ���������',0
.3504:db '⮫���',0
.3505:db '���',0
.3506:db '⮭���',0
.3507:db '�㬠��',0
.3508:db '��᫨⥫�',0
.3509:db '��⨩',0
.3510:db '�ਭ�����',0
.3511:db '�ਭ�����',0
.3512:db '�ਤ���',0
.3513:db '�ਤ���',0
.3514:db '���, ��, ��',0
.3515:db '�',0
.3516:db '�㬠��',0
.3517:db '�����',0
.3518:db '(��)������',0
.3519:db '��૮, ���⪠',0
.3520:db '�����, ������',0
.3521:db '�모�뢠��',0
.3522:db '�����',0
.3523:db '�����⪨',0
.3524:db '�������',0
.3525:db '������',0
.3526:db '⨣�',0
.3527:db '�㣮�, �९���, ���',0
.3528:db '�������(��)',0
.3529:db '�९��, ��䥫�',0
.3530:db '��',0
.3531:db '�६�',0
.3532:db '�ᯨᠭ��',0
.3533:db '�����, �⪨�뢠��, 砥��',0
.3534:db '���������, ��������',0
.3535:db '��, �, �, ��',0
.3536:db 'ᥣ����',0
.3537:db '����� ����, ��᮪ (�㫪�, ��謠��)',0
.3538:db '�����',0
.3539:db '��㤨����, �殮�� ���',0
.3540:db '�㠫��',0
.3541:db '����, �ਬ��, �ਧ���',0
.3542:db '��᪠����, ᪠����',0
.3543:db '᭮��, �����⨬�',0
.3544:db '�௨��',0
.3545:db '�������',0
.3546:db '������, �஡���',0
.3547:db '�����',0
.3548:db '��',0
.3549:db '⮦�, ᫨誮�',0
.3550:db '����, �����',0
.3551:db '����㯨� �� ᮡ࠭��',0
.3552:db '�����㬥��, ��㤨�',0
.3553:db '�����㬥���',0
.3554:db '�����誠, ��設�',0
.3555:db '(�� ���ࢫ����) �',0
.3556:db '(�� ���ࢫ����) �',0
.3557:db '����⥭�',0
.3558:db '����',0
.3559:db '��த',0
.3560:db '����誠',0
.3561:db '᫥�, ��᫥����, �����',0
.3562:db '᫥�, �.-�. �����, �ய����, ��ண�, ��᫥������',0
.3563:db '�ࠪ��',0
.3564:db '�ࠪ����',0
.3565:db '�࣮���, ६�᫮, ������',0
.3566:db '�����, �࣮����',0
.3567:db '�ࠤ���, �।����',0
.3568:db '㫨筮� ��������, �࠭ᯮ��',0
.3569:db '�ࠣ����',0
.3570:db '�ࠣ��᪨�',0
.3571:db '��ᯨ�뢠��, �७�஢�����',0
.3572:db '�७��',0
.3573:db '�ࠬ���',0
.3574:db '��ॢ�����(��)',0
.3575:db '��ॢ��',0
.3576:db '��।��',0
.3577:db '�஧���',0
.3578:db '��ॢ����, �࠭ᯮ��, ��ॢ�����',0
.3579:db '�������, ����誠, ������',0
.3580:db '��',0
.3581:db '�����⢮����, ��।��������, �����⢨�',0
.3582:db '�����⢥����',0
.3583:db '���ᥪ���',0
.3584:db '������',0
.3585:db '�������',0
.3586:db '��ॢ�',0
.3587:db '�����⢨�',0
.3588:db '�஫������',0
.3589:db '����',0
.3590:db '���᪠',0
.3591:db '��, �⠭�',0
.3592:db '��㧮���, �����-�����ଠ',0
.3593:db '����ਥ',0
.3594:db '�ࠢ��',0
.3595:db '�������, �஡�����',0
.3596:db '�ਬ����',0
.3597:db '�����, ��堭�, ���',0
.3598:db '��㡪�, ��',0
.3599:db '��쯠�',0
.3600:db '�������',0
.3601:db '�㭥��',0
.3602:db '���',0
.3603:db '������, �����稢���',0
.3604:db '�몫����',0
.3605:db '�������',0
.3606:db '��୮��',0
.3607:db '⮪���',0
.3608:db '��������',0
.3609:db '���������',0
.3610:db '�������',0
.3611:db '�������',0
.3612:db '������� ����',0
.3613:db '������� ����',0
.3614:db '������',0
.3615:db '���',0
.3616:db '����, ����誠',0
.3617:db '����ᨢ�',0
.3618:db '�����⥫��',0
.3619:db '���⨪',0
.3620:db '����।��񭭮���, �������⭮���, ��㢥७�����',0
.3621:db '���',0
.3622:db '��㤮���',0
.3623:db '����몭������',0
.3624:db '���',0
.3625:db '��������, ��������',0
.3626:db '����ન����',0
.3627:db '����ભ���',0
.3628:db '��������',0
.3629:db '������',0
.3630:db '�ଠ, �ଥ���� ������',0
.3631:db '��',0
.3632:db '������, ����᪠� ����',0
.3633:db 'ᮥ������(��), ��ꥤ�����(��)',0
.3634:db '�����⢮',0
.3635:db '����⢥���� �ਧ�����',0
.3636:db '㭨������',0
.3637:db '���������',0
.3638:db '��ᯮ����⢮, ��������',0
.3639:db '������, �����',0
.3640:db '����� � ����, ���� � ���।',0
.3641:db '��ᯨ⠭��',0
.3642:db '���孨�, ���訩',0
.3643:db '��, ��������',0
.3644:db '���뢠�� � ��୥�, �᪮७���',0
.3645:db '���ࢫ���� �����',0
.3646:db '�����',0
.3647:db '����, �����, �����⥫��',0
.3648:db '�ୠ',0
.3649:db '���, ���',0
.3650:db '㯮�ॡ�����, ���砩',0
.3651:db '���짠, ���짮�����, 㯮�ॡ����',0
.3652:db '�������, �ਣ����',0
.3653:db '��ᯮ�����',0
.3654:db '������, �����줨���',0
.3655:db '�����, ��몭������',0
.3656:db '���筮',0
.3657:db '�ᯮ�짮����',0
.3658:db 'ᮢ��襭��',0
.3659:db '�����, ���⮥ ����࠭�⢮, �������',0
.3660:db '���⮩, �������, ᢮�����, �����, �����᫥��� (�����)',0
.3661:db '�������, ����. ����',0
.3662:db '�뫥��',0
.3663:db '����, �����, ����।����',0
.3664:db '����, ���⮩, ��᫠���, ᠬ��������',0
.3665:db '��. ����⢨⥫��, ����騩 ᨫ�, ��᪨�, ���᭮�����',0
.3666:db '������',0
.3667:db '業��, �ࠣ�業����',0
.3668:db '業�����, �. �⮨�����, ���. ����稭�, �業�����, 業���',0
.3669:db '������',0
.3670:db '��࣮�, ������� �����, �������',0
.3671:db '�������',0
.3672:db '��祧���',0
.3673:db '��᫠���',0
.3674:db '���������, �८��������',0
.3675:db '���, ����',0
.3676:db 'ࠧ����ࠧ��, ࠧ����������, �����',0
.3677:db '�����, ���ࠤ�� ⥠��',0
.3678:db 'ࠧ����, ࠧ��',0
.3679:db '���, ���, ����஢���',0
.3680:db '(��)��������, ࠧ����ࠧ���',0
.3681:db '����',0
.3682:db '�����, �஬����',0
.3683:db '⥫�⨭�',0
.3684:db '���⥫��, ����',0
.3685:db '������, ����⮢�',0
.3686:db '����, �������',0
.3687:db '�㠫�, ���㠫�஢���',0
.3688:db '����, �����',0
.3689:db '�����',0
.3690:db '�����, �饭��',0
.3691:db '�஢��ਢ���, ���⨫�஢���',0
.3692:db '�᪭���, �⢠������',0
.3693:db '�ࠬ. ������',0
.3694:db '����, �ࠬ. ���������',0
.3695:db '�ਣ����',0
.3696:db '������',0
.3697:db '�࠭�, ��७. �ࠩ',0
.3698:db '�࠭����, ���� �� �࠭�',0
.3699:db '�஢�����',0
.3700:db '���(�), ����',0
.3701:db '���⨪����',0
.3702:db '�祭�, ���쬠',0
.3703:db '�祭� �����',0
.3704:db '�祭� ���',0
.3705:db '���, ��ࠡ��, �㤭�',0
.3706:db '����. �����, �����',0
.3707:db '���࠭',0
.3708:db 'ࠧ�ࠦ���, �न��',0
.3709:db '��ᠤ�, ࠧ�ࠦ����',0
.3710:db '����஢���',0
.3711:db '��ப',0
.3712:db '���',0
.3713:db '���᭮��, �ᥤ�⢮, ��������',0
.3714:db '����, ������',0
.3715:db '���⢠',0
.3716:db '����������',0
.3717:db '������',0
.3718:db '�஢����, �த������⢨�',0
.3719:db '���, ������, �ᬠ�ਢ���, ��ᬠ�ਢ���',0
.3720:db '���⥫쭮���',0
.3721:db 'ᨫ��, �ࣨ��',0
.3722:db '�����',0
.3723:db '��ॢ��, ᥫ�',0
.3724:db 'ᥫ�᪨� ��⥫�',0
.3725:db '������, ������',0
.3726:db '��ࠢ�뢠��',0
.3727:db '�����ࠤ��� ����',0
.3728:db '����',0
.3729:db '�����ࠤ���',0
.3730:db '�������, �������, ��ᨫ�����',0
.3731:db 'ᨫ�, ����⮢�⢮, ��ᨫ��',0
.3732:db 'ᨫ��, ����⮢�, �㩭�, ��ᨫ��⢥���',0
.3733:db '䨠���, 䨮��⮢�',0
.3734:db '�ਯ��',0
.3735:db '���',0
.3736:db '���த�⥫�, ���⮨��⢮',0
.3737:db '������, �祢����',0
.3738:db '�७��, �������',0
.3739:db '������, �����, ���饭��',0
.3740:db '�����, ����⥫�',0
.3741:db '��⥫��, ����來�',0
.3742:db '��������, ������, �����',0
.3743:db '�����, �ન�',0
.3744:db '᫮����, ����� ᫮�',0
.3745:db '����ᮢ��, �������',0
.3746:db '�ਧ�����',0
.3747:db '����ᨮ�����',0
.3748:db '������稫��',0
.3749:db '�����',0
.3750:db '��襭��, ��. ������⢨⥫��',0
.3751:db '�㫪��',0
.3752:db '��������',0
.3753:db '�����������',0
.3754:db '����',0
.3755:db '(���᪮�) �����⢨�',0
.3756:db '�����',0
.3757:db '��樠��',0
.3758:db '���㦨����',0
.3759:db '��樠�⪠',0
.3760:db '�ப�������, ����',0
.3761:db '�㤨��',0
.3762:db '�ண㫪� ��誮�, �����',0
.3763:db '�⥭�',0
.3764:db '������⮪, �����⢨�, �㦤�, ����',0
.3765:db '�����, ������',0
.3766:db '��थ஡, 誠�',0
.3767:db '⥯��',0
.3768:db 'ࠤ��� �ਥ�',0
.3769:db '�������稥, �थ�, ��࠭�஢���',0
.3770:db '����',0
.3771:db '������ ��ࠡ��',0
.3772:db '��, �뫠',0
.3773:db '����(��), �뢠��(��)',0
.3774:db '���� �����',0
.3775:db '᫥����, ������, ��஦���, ���⥫쭮���',0
.3776:db 'ᬮ���� ⥫������',0
.3777:db '���⥫��',0
.3778:db '��筮� ��஦',0
.3779:db '����, ��������',0
.3780:db '����५�',0
.3781:db '���',0
.3782:db '�������',0
.3783:db '�������, �����',0
.3784:db '�����',0
.3785:db '���஬������, ���஬������ (����)',0
.3786:db '�����, �����, ���������, ࠧ��������, �����, ��������(��)',0
.3787:db '���, ��᪮���',0
.3788:db '��ண�, ����, �����, ᯮᮡ',0
.3789:db '��',0
.3790:db '���� ��אַ ᬮ���� �ࠢ�� � ���',0
.3791:db '������⢮',0
.3792:db '����� ������',0
.3793:db '������',0
.3794:db '������',0
.3795:db '��ࠡ�祥 �६�',0
.3796:db '��������쭮, ����������',0
.3797:db '�������',0
.3798:db '�����, ����訢���(��), ����� ���, ���祭��',0
.3799:db '���, �殮���, ���, ����, ���祭��',0
.3800:db '��᪨�',0
.3801:db '�ਢ���⢨�, �������, �ਢ���⢮����, ࠤ�譮 �������',0
.3802:db '���������稥',0
.3803:db '�������, த���, ���',0
.3804:db '������',0
.3805:db '���⠭��',0
.3806:db '����⥫��, �������',0
.3807:db '��襫',0
.3808:db '�뫨',0
.3809:db '�����',0
.3810:db '�������',0
.3811:db '�����, ������, ��������, ᬠ稢���, 㢫������',0
.3812:db '���',0
.3813:db '��, �����',0
.3814:db '�� �� ��� �⨬ ᪠����',0
.3815:db '�� ��, �� �� ��',0
.3816:db '�襭��',0
.3817:db '�����, ���, ���ࢠ�, �����, ����',0
.3818:db '�窠',0
.3819:db '�����',0
.3820:db '��直� ࠧ ���, ����� �� ��',0
.3821:db '���, �㤠',0
.3822:db '�ਭ���� �� ��������, ⮣�� ���',0
.3823:db '����� ��, �㤠�� ��',0
.3824:db '��, ���',0
.3825:db '�����, �����',0
.3826:db '�� �� ���',0
.3827:db '����',0
.3828:db '���㤠, ���ਧ',0
.3829:db '����',0
.3830:db '����, ����, �������� (᫨���, ��)',0
.3831:db '�����',0
.3832:db '���������, ��� (� �������)',0
.3833:db '����',0
.3834:db '��, �����',0
.3835:db '����, 楫�',0
.3836:db '祩',0
.3837:db '��祬�',0
.3838:db '�ப��',0
.3839:db '�����',0
.3840:db '������',0
.3841:db '�ਭ�',0
.3842:db '����',0
.3843:db '��ਪ',0
.3844:db '�����',0
.3845:db '������� 梥⮪',0
.3846:db '������, �����',0
.3847:db '���������, �먣�뢠��',0
.3848:db '����',0
.3849:db '����',0
.3850:db '������� �⥪��',0
.3851:db '���७��',0
.3852:db '����',0
.3853:db '�����, ��',0
.3854:db '����',0
.3855:db 'ࠤ��',0
.3856:db '������',0
.3857:db '����, �����ࠧ㬭�',0
.3858:db '������, ����, �������',0
.3859:db '�, ����� �',0
.3860:db '��뫠��� ��, �⭮�⥫쭮',0
.3861:db '�����, � �।���� (祣�-�.)',0
.3862:db '���',0
.3863:db '��������',0
.3864:db '���騭�, ���᪨�',0
.3865:db '���騭�',0
.3866:db '�먣���',0
.3867:db '�㤮, 㤨�������',0
.3868:db '�㤥��',0
.3869:db '���, ��ॢ� (���ਠ�), �஢�',0
.3870:db '��ॢﭭ�',0
.3871:db '��⥫',0
.3872:db 'ࠡ�� �� ��ॢ�',0
.3873:db '�����',0
.3874:db '����ﭮ�',0
.3875:db '᫮��',0
.3876:db '᫮�� ��⠭��',0
.3877:db '����� (������)',0
.3878:db 'ࠡ��, ࠡ����',0
.3879:db 'ࠡ�稩',0
.3880:db '����騥��',0
.3881:db '�����᪠�',0
.3882:db '���',0
.3883:db '�㦨�',0
.3884:db '�����訩',0
.3885:db '࠭����',0
.3886:db '࠭���',0
.3887:db '�����',0
.3888:db '�����뢠��',0
.3889:db '���⥫�',0
.3890:db '���ࠢ����',0
.3891:db '�������',0
.3892:db '�ᨫ�䮭',0
.3893:db '���',0
.3894:db '��',0
.3895:db '���୨�',0
.3896:db '������',0
.3897:db '���',0
.3898:db '�஦��',0
.3899:db '������',0
.3900:db '�����',0
.3901:db '��',0
.3902:db '���',0
.3903:db '��',0
.3904:db '�ਭ����',0
.3905:db '�ମ',0
.3906:db '���⮪',0
.3907:db '��',0
.3908:db '�������',0
.3909:db '���',0
.3910:db 'ᠬ',0
.3911:db 'ᥡ�, ᠬ�',0
.3912:db '���������',0
.3913:db '��न�',0
.3914:db '�����',0
.3915:db '���',0
.3916:db '樭�',0
.3917:db '���⥦��-������',0
.3918:db '���⥦��-������',0
.3919:db '����',0
.3920:db '������',0
.3921:db '��������',0
