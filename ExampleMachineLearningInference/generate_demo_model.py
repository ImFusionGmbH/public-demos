# Script to generate the model used for the demo
import torch
import torch.nn
from torchvision.models.segmentation import fcn_resnet101

# we create a wrapper class so that the output of the model is a single tensor and not a dictionary
class FCNResNet101Model(torch.nn.Module):
    def __init__(self):
        super(FCNResNet101Model, self).__init__()
        self.model = fcn_resnet101(pretrained=True, progress=False)
        self.MEAN = torch.tensor([0.485, 0.456, 0.406])
        self.STD = torch.tensor([0.229, 0.224, 0.225])

    def forward(self, x):        
        
        # Pre-process the tensor with ImageNet normalization
        for c in range(3):
            x[:,c] = (x[:,c]-self.MEAN[c]) / self.STD[c]

        outputs = self.model(x)
        return outputs['out']

model = FCNResNet101Model()
model = model.eval()

dummy_input = torch.rand(1, 3, 256, 256) # batch x channels x height x width
traced_script_module = torch.jit.trace(model, dummy_input)
traced_script_module.save("demo_model_traced.pt")
# the demo_model.yaml file must be generated manually following the tutorial