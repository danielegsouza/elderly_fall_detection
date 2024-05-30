# Avaliação de Desempenho de um Sistema Embarcado na Detecção de Queda de Idosos

## Descrição

Este projeto visa desenvolver um sistema de detecção de quedas em idosos utilizando o ESP32-E-WROVER equipado com o sensor MPU6500. O objetivo é monitorar a mobilidade dos idosos e alertar em caso de eventos de queda, melhorando assim a segurança e independência das pessoas idosas.

## Tecnologias Utilizadas

- **Hardware**: ESP32-E-WROVER, MPU6500
- **Software**: Python (para pré-processamento e treinamento de modelos), Arduino IDE (para programação do ESP32)
- **Bibliotecas**: FastIMU (para leitura de dados do sensor), emlearn (para conversão de modelos Python para C/C++), micromlgen (para geração de código de modelo)

## Estrutura do Projeto

O projeto é organizado em vários arquivos e notebooks Jupyter:

- `pre-processing.ipynb`: Realiza o pré-processamento do dataset SisFall.
- `random-forest.ipynb`: Contém treinamentos de modelos para avaliação.
- `random_forest_balanced_class.py`: Treinamento com o dataset com classes balanceadas e conversão do modelo Python para emlearn e micromlgen.
- `fall_detection_esp.ino`: Arquivo principal para o ESP32 que lê os dados do sensor com a biblioteca FastIMU, realiza inferência com o modelo e envia alertas via Telegram.
- `random_forest.h`: Arquivo gerado após a aplicação da biblioteca emlearn, contendo o modelo otimizado para execução no ESP32.


