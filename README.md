![image](https://github.com/RICS-UNINOVA/Gripper-Linear/assets/74061652/ca27036d-2340-4906-ac9f-d6a06c8aadff)



Para utilizar a gripper é necessario chamar os serviços disponibiliazdos.

IMPORTANTE: ANTES DE CHAMAR QUALQUER SERVIÇO OU ATE MESMO DE LIGAR A GRIPPER AS PINÇAS DEVEM-SE ENCONTRAR COMPLETAMENTE FECHADAS. AMBAS AS PONTAS A TOCAR UMA NA OUTRA AO CENTRO DA CARCAÇA DA GRIPPER.

Rede: RICS_PUB

IP: 192.168.250.21

Serviços:

  Close: faz fechar a gripper e deve ser passado como parametro a dimensao (em mm) do objeto a apanhar.
  
  Open: Faz abrir totalmente a gripper e nao necessita de parametros.
  

Como chamar o serviço:
  Close:
    http://192.168.250.21/close?dimension=58
    
  Open:
    http://192.168.250.21/open

Notas: As peças do Kit FMS têm uma dimensao de 58mm.
