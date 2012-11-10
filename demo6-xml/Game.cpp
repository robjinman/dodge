#include <cstdio>
#include <sys/time.h>
#include <sstream>
#include <iostream>
#include <fstream>
#include <sstream>
#include "Game.hpp"


using namespace Dodge;
using namespace boost;
using namespace rapidxml;
using namespace std;


void Game::quit() {
   exit(0);
}

void Game::keyUp(int code) {
   m_keys[code] = false;

   // dirKeyStack is the list of currently depressed directional
   // keys in the order they were pressed.
   for (uint_t i = 0; i < m_dirKeyStack.size(); ++i) {
      if (m_keys[m_dirKeyStack[i]] == false) {
         m_dirKeyStack.erase(m_dirKeyStack.begin() + i);
         --i;
      }
   }
}

void Game::keyDown(int code) {
   if (code == WinIO::KEY_LEFT || code == WinIO::KEY_RIGHT
      || code == WinIO::KEY_UP || code == WinIO::KEY_DOWN) {

      m_dirKeyStack.push_back(code);
   }

   m_keys[code] = true;

   switch (code) {
      case WinIO::KEY_ESCAPE: quit(); break;
   }
}

void Game::keyboard() {
   static long strMoveDown = internString("moveDown");
   static long strMoveLeft = internString("moveLeft");
   static long strMoveUp = internString("moveUp");
   static long strMoveRight = internString("moveRight");

   static int missedKeypress = -1;

   // Attempt to move in direction of missed keypress
   if (missedKeypress != -1) {
      switch (missedKeypress) {
         case WinIO::KEY_RIGHT:
            if (m_player->playAnimation(strMoveRight)) {
               m_player->playTransformation(strMoveRight);
               missedKeypress = -1;
            }
         break;
         case WinIO::KEY_LEFT:
            if (m_player->playAnimation(strMoveLeft)) {
               m_player->playTransformation(strMoveLeft);
               missedKeypress = -1;
            }
         break;
         case WinIO::KEY_UP:
            if (m_player->playAnimation(strMoveUp)) {
               m_player->playTransformation(strMoveUp);
               missedKeypress = -1;
            }
         break;
         case WinIO::KEY_DOWN:
            if (m_player->playAnimation(strMoveDown)) {
               m_player->playTransformation(strMoveDown);
               missedKeypress = -1;
            }
         break;
      }
   }
   else {
      // Move in the direction of most recently pressed key
      if (m_dirKeyStack.size() > 0) {
         switch (m_dirKeyStack.back()) {
            case WinIO::KEY_RIGHT:
               if (m_player->playAnimation(strMoveRight)) {
                  m_player->playTransformation(strMoveRight);
               }
               else {   // If the player is already moving remember this keypress
                  if (m_player->getAnimName() != strMoveRight)
                     missedKeypress = WinIO::KEY_RIGHT;
               }
            break;
            case WinIO::KEY_LEFT:
               if (m_player->playAnimation(strMoveLeft)) {
                  m_player->playTransformation(strMoveLeft);
               }
               else {
                  if (m_player->getAnimName() != strMoveLeft)
                     missedKeypress = WinIO::KEY_LEFT;
               }
            break;
            case WinIO::KEY_UP:
               if (m_player->playAnimation(strMoveUp)) {
                  m_player->playTransformation(strMoveUp);
               }
               else {
                  if (m_player->getAnimName() != strMoveUp)
                     missedKeypress = WinIO::KEY_UP;
               }
            break;
            case WinIO::KEY_DOWN:
               if (m_player->playAnimation(strMoveDown)) {
                  m_player->playTransformation(strMoveDown);
               }
               else {
                  if (m_player->getAnimName() != strMoveDown)
                     missedKeypress = WinIO::KEY_DOWN;
               }
            break;
         }
      }
   }
}

void Game::computeFrameRate() {
   static long i = 0;
   ++i;

   if (i % 100 == 0) {
      static timeval now;
      static timeval prev;

      gettimeofday(&now, NULL);

      double t = now.tv_sec - prev.tv_sec;
      t += static_cast<double>(now.tv_usec - prev.tv_usec) / 1000000.0;

      if (i > 100)
         m_frameRate = 100.0 / t;

      prev = now;
   }
}

void Game::playerSetup() {/*
   float32_t w = 32.f * gGetPixelSize().x;
   float32_t h = 32.f * gGetPixelSize().y;

   pTexture_t texMan(new Texture("data/textures/man.png"));
   m_player = pSprite_t(new Sprite(internString("m_player"), internString("man"), texMan));

   vector<AnimFrame> aFrames;
   aFrames.push_back(AnimFrame(Vec2f(0.f, 0.f), Vec2f(32.f, 32.f), Colour(1.0, 1.0, 1.0, 1.0)));
   aFrames.push_back(AnimFrame(Vec2f(0.f, 32.f), Vec2f(32.f, 32.f), Colour(1.0, 1.0, 1.0, 1.0)));
   aFrames.push_back(AnimFrame(Vec2f(0.f, 64.f), Vec2f(32.f, 32.f), Colour(1.0, 1.0, 1.0, 1.0)));
   aFrames.push_back(AnimFrame(Vec2f(0.f, 96.f), Vec2f(32.f, 32.f), Colour(1.0, 1.0, 1.0, 1.0)));
   pAnimation_t animMoveDown(new Animation(internString("moveDown"), 16.f, aFrames));

   aFrames.clear();
   aFrames.push_back(AnimFrame(Vec2f(32.f, 0.f), Vec2f(32.f, 32.f), Colour(1.0, 1.0, 1.0, 1.0)));
   aFrames.push_back(AnimFrame(Vec2f(32.f, 32.f), Vec2f(32.f, 32.f), Colour(1.0, 1.0, 1.0, 1.0)));
   aFrames.push_back(AnimFrame(Vec2f(32.f, 64.f), Vec2f(32.f, 32.f), Colour(1.0, 1.0, 1.0, 1.0)));
   aFrames.push_back(AnimFrame(Vec2f(32.f, 96.f), Vec2f(32.f, 32.f), Colour(1.0, 1.0, 1.0, 1.0)));
   pAnimation_t animMoveLeft(new Animation(internString("moveLeft"), 16.f, aFrames));

   aFrames.clear();
   aFrames.push_back(AnimFrame(Vec2f(64.f, 0.f), Vec2f(32.f, 32.f), Colour(1.0, 1.0, 1.0, 1.0)));
   aFrames.push_back(AnimFrame(Vec2f(64.f, 32.f), Vec2f(32.f, 32.f), Colour(1.0, 1.0, 1.0, 1.0)));
   aFrames.push_back(AnimFrame(Vec2f(64.f, 64.f), Vec2f(32.f, 32.f), Colour(1.0, 1.0, 1.0, 1.0)));
   aFrames.push_back(AnimFrame(Vec2f(64.f, 96.f), Vec2f(32.f, 32.f), Colour(1.0, 1.0, 1.0, 1.0)));
   pAnimation_t animMoveUp(new Animation(internString("moveUp"), 16.f, aFrames));

   aFrames.clear();
   aFrames.push_back(AnimFrame(Vec2f(96.f, 0.f), Vec2f(32.f, 32.f), Colour(1.0, 1.0, 1.0, 1.0)));
   aFrames.push_back(AnimFrame(Vec2f(96.f, 32.f), Vec2f(32.f, 32.f), Colour(1.0, 1.0, 1.0, 1.0)));
   aFrames.push_back(AnimFrame(Vec2f(96.f, 64.f), Vec2f(32.f, 32.f), Colour(1.0, 1.0, 1.0, 1.0)));
   aFrames.push_back(AnimFrame(Vec2f(96.f, 96.f), Vec2f(32.f, 32.f), Colour(1.0, 1.0, 1.0, 1.0)));
   pAnimation_t animMoveRight(new Animation(internString("moveRight"), 16.f, aFrames));

   m_player->addAnimation(animMoveDown);
   m_player->addAnimation(animMoveLeft);
   m_player->addAnimation(animMoveUp);
   m_player->addAnimation(animMoveRight);

   vector<TransFrame> fFrames;
   fFrames.push_back(TransFrame(Vec2f(0.f, -h), 0.f, Vec2f(1.f, 1.f)));
   pTransformation_t transMoveDown(new Transformation(internString("moveDown"), 4.f, fFrames));
   transMoveDown->setSmooth(8);

   fFrames.clear();
   fFrames.push_back(TransFrame(Vec2f(-w, 0.f), 0.f, Vec2f(1.f, 1.f)));
   pTransformation_t transMoveLeft(new Transformation(internString("moveLeft"), 4.f, fFrames));
   transMoveLeft->setSmooth(8);

   fFrames.clear();
   fFrames.push_back(TransFrame(Vec2f(0.f, h), 0.f, Vec2f(1.f, 1.f)));
   pTransformation_t transMoveUp(new Transformation(internString("moveUp"), 4.f, fFrames));
   transMoveUp->setSmooth(8);

   fFrames.clear();
   fFrames.push_back(TransFrame(Vec2f(w, 0.f), 0.f, Vec2f(1.f, 1.f)));
   pTransformation_t transMoveRight(new Transformation(internString("moveRight"), 4.f, fFrames));
   transMoveRight->setSmooth(8);

   m_player->addTransformation(transMoveDown);
   m_player->addTransformation(transMoveLeft);
   m_player->addTransformation(transMoveUp);
   m_player->addTransformation(transMoveRight);

   m_player->setShape(unique_ptr<Primitive>(new Quad(Vec2f(0.f, 0.f), Vec2f(w, 0.f), Vec2f(w, h), Vec2f(0.f, h))));

   m_player->setTranslation(w * 7.f, h * 5.f);
   m_player->setZ(2);

   m_worldSpace.trackEntity(m_player);*/
}

void Game::buttonPressHandler(pEntity_t entity) {
   std::cout << "Button Pressed!\n";
}

void Game::buttonReleaseHandler(pEntity_t entity) {
   std::cout << "Button Released!\n";
}

void Game::uiSetup() {/*
   pTexture_t tex(new Texture("data/textures/ss8x8squares128x128y.png"));
   pUiButton_t btn(new UiButton(internString("button1"), tex));

   vector<AnimFrame> aFrames;
   aFrames.push_back(AnimFrame(Vec2f(0.f, 0.f), Vec2f(16.f, 16.f), Colour(1.0, 1.0, 1.0, 1.0)));
   aFrames.push_back(AnimFrame(Vec2f(16.f, 0.f), Vec2f(16.f, 16.f), Colour(1.0, 1.0, 1.0, 1.0)));
   aFrames.push_back(AnimFrame(Vec2f(32.f, 0.f), Vec2f(16.f, 16.f), Colour(1.0, 1.0, 1.0, 1.0)));
   aFrames.push_back(AnimFrame(Vec2f(48.f, 0.f), Vec2f(16.f, 16.f), Colour(1.0, 1.0, 1.0, 1.0)));
   pAnimation_t btnPress(new Animation(internString("press"), 16.f, aFrames));

   aFrames.clear();
   aFrames.push_back(AnimFrame(Vec2f(0.f, 16.f), Vec2f(16.f, 16.f), Colour(1.0, 1.0, 1.0, 1.0)));
   aFrames.push_back(AnimFrame(Vec2f(16.f, 16.f), Vec2f(16.f, 16.f), Colour(1.0, 1.0, 1.0, 1.0)));
   aFrames.push_back(AnimFrame(Vec2f(32.f, 16.f), Vec2f(16.f, 16.f), Colour(1.0, 1.0, 1.0, 1.0)));
   aFrames.push_back(AnimFrame(Vec2f(48.f, 16.f), Vec2f(16.f, 16.f), Colour(1.0, 1.0, 1.0, 1.0)));
   pAnimation_t btnRelease(new Animation(internString("release"), 16.f, aFrames));

   aFrames.clear();
   aFrames.push_back(AnimFrame(Vec2f(0.f, 32.f), Vec2f(16.f, 16.f), Colour(1.0, 1.0, 1.0, 1.0)));
   aFrames.push_back(AnimFrame(Vec2f(16.f, 32.f), Vec2f(16.f, 16.f), Colour(1.0, 1.0, 1.0, 1.0)));
   aFrames.push_back(AnimFrame(Vec2f(32.f, 32.f), Vec2f(16.f, 16.f), Colour(1.0, 1.0, 1.0, 1.0)));
   aFrames.push_back(AnimFrame(Vec2f(48.f, 32.f), Vec2f(16.f, 16.f), Colour(1.0, 1.0, 1.0, 1.0)));
   pAnimation_t hoverOn(new Animation(internString("hoverOn"), 16.f, aFrames));

   aFrames.clear();
   aFrames.push_back(AnimFrame(Vec2f(0.f, 48.f), Vec2f(16.f, 16.f), Colour(1.0, 1.0, 1.0, 1.0)));
   aFrames.push_back(AnimFrame(Vec2f(16.f, 48.f), Vec2f(16.f, 16.f), Colour(1.0, 1.0, 1.0, 1.0)));
   aFrames.push_back(AnimFrame(Vec2f(32.f, 48.f), Vec2f(16.f, 16.f), Colour(1.0, 1.0, 1.0, 1.0)));
   aFrames.push_back(AnimFrame(Vec2f(48.f, 48.f), Vec2f(16.f, 16.f), Colour(1.0, 1.0, 1.0, 1.0)));
   pAnimation_t hoverOff(new Animation(internString("hoverOff"), 16.f, aFrames));

   btn->addAnimation(btnPress);
   btn->addAnimation(btnRelease);
   btn->addAnimation(hoverOn);
   btn->addAnimation(hoverOff);

   float32_t w = 16.f * gGetPixelSize().x;
   float32_t h = 16.f * gGetPixelSize().y;

   btn->setShape(unique_ptr<Primitive>(new Quad(Vec2f(0.f, 0.f), Vec2f(w, 0.f), Vec2f(w, h), Vec2f(0.f, h))));
   btn->setTranslation(0.1f, 0.15f);

   btn->setOnPressHandler(Functor<void, TYPELIST_1(pEntity_t)>(this, &Game::buttonPressHandler));
   btn->setOnReleaseHandler(Functor<void, TYPELIST_1(pEntity_t)>(this, &Game::buttonReleaseHandler));

   m_entities.push_back(btn);*/
}

boost::shared_ptr<Asset> Game::constructAsset(const XmlNode data, long proto) {
   // if proto = -1 asset is *not* constructed from prototype

   boost::shared_ptr<Asset> asset;

   if (data.name() == "Texture") {
      asset = boost::shared_ptr<Asset>(new Texture(data));
   }
   else if (data.name() == "Player") {
      if (proto == -1) {
         asset = pPlayer_t(new Player(data));
      }
      else {
         Player* player = dynamic_cast<Player*>(m_assetManager.cloneAsset(proto));

         if (!player)
            throw Exception("Error constructing asset of type Player; Bad prototype id", __FILE__, __LINE__);

         m_player = pPlayer_t(player);
         m_player->assignData(data);

         m_player->addToWorld();
         m_worldSpace.trackEntity(m_player);

         asset = m_player;
      }
   }
   else if (data.name() == "Soil") {/*
      pSoil_t soil(new Soil(data));
      soil->addToWorld();
      m_worldSpace.trackEntity(soil);
      m_items.push_back(soil);

      asset = soil;*/
   }
   else
      throw Exception("Error constructing entity; Unrecognised type", __FILE__, __LINE__);

   return asset;
}

void Game::loadAssets_r(const string& file) {
   XmlNode decl = parseXmlDocument(file);
   XmlNode node = decl.nextSibling();

   if (node.isNull() || node.name() != "ASSETFILE")
      throw XmlException("Error parsing XML file; expected 'ASSETFILE' tag", __FILE__, __LINE__);

   node = node.firstChild();

   if (node.isNull())
      throw XmlException("Error parsing XML file; expected 'using' or 'assets' tag", __FILE__, __LINE__);

   if (node.name() == "using") {
      XmlNode node_ = node.firstChild();
      while (!node_.isNull() && node_.name() == "file") {
         string path = string("data/xml/").append(node_.value());
         loadAssets_r(path);

         node_ = node_.nextSibling();
      }

      node = node.nextSibling();
   }

   if (node.isNull() || node.name() != "assets")
      throw XmlException("Error parsing XML file; expected 'assets' tag", __FILE__, __LINE__);

   node = node.firstChild();
   while (!node.isNull() && node.name() == "asset") {
      XmlAttribute attr = node.firstAttribute();
      if (attr.isNull() || attr.name() != "id")
         throw XmlException("Error parsing XML file; expected 'id' attribute", __FILE__, __LINE__);

      long id = 0;
      long proto = -1;
      sscanf(attr.value().data(), "%ld", &id);

      attr = attr.nextAttribute();
      if (!attr.isNull()) sscanf(attr.value().data(), "%ld", &proto);

      boost::shared_ptr<Asset> asset = constructAsset(node.firstChild(), proto);
      m_assetManager.addAsset(id, asset);

      node = node.nextSibling();
   }
}

void Game::init() {
   m_win.init("Shit Game", 640, 480, false);
   m_win.registerCallback(WinIO::EVENT_WINCLOSE, Functor<void, TYPELIST_0()>(this, &Game::quit));
   m_win.registerCallback(WinIO::EVENT_KEYDOWN, Functor<void, TYPELIST_1(int)>(this, &Game::keyDown));
   m_win.registerCallback(WinIO::EVENT_KEYUP, Functor<void, TYPELIST_1(int)>(this, &Game::keyUp));

   m_worldSpace.init(unique_ptr<Quadtree<pEntity_t> >(new Quadtree<pEntity_t>(1, Range(0.f, 0.f, 64.f / 48.f, 1.f))));

   m_graphics2d.init(640, 480);

   m_currentMap = 0;

   stringstream strMap;
   strMap << "data/xml/map" << m_currentMap << ".xml";
   loadAssets_r(strMap.str());

   playerSetup();
   uiSetup();

   pTexture_t texFont1(new Texture("data/textures/font2.png"));
   m_font1 = pFont_t(new Dodge::Font(texFont1, 0, 0, 852, 792, 71, 98));
}

void Game::launch() {
   init();

   while (true) {
      m_win.doEvents();
      keyboard();
      computeFrameRate();

      m_graphics2d.clear(Colour(0.5, 0.6, 0.8, 1.0));
      m_worldSpace.dbg_draw(5, Colour(1.f, 0.f, 0.f, 1.f));

      for (uint_t i = 0; i < m_items.size(); ++i) {
         m_items[i]->update();
         m_items[i]->draw();
      }

      m_player->update();
      m_player->draw();

      stringstream strFr;
      strFr << "Frame Rate: " << m_frameRate << "fps";
      m_graphics2d.setFillColour(Colour(1.f, 0.f, 0.f, 1.f));
      m_graphics2d.drawText(*m_font1, strFr.str(), 0.03f, 460.f * gGetPixelSize().y, 5, 0.f, Vec2f(0.f, 0.f), Vec2f(0.15f, 0.15f));

      m_eventManager.doEvents();

      m_win.swapBuffers();
   }
}
